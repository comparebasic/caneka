#include <external.h>
#include <caneka.h>

static char *flagStrs[] = {
    "PAT_TERM",
    "PAT_OPTIONAL",
    "PAT_MANY",
    "PAT_ANY",
    "PAT_INVERT",
    "PAT_COUNT",
    "PAT_INVERT_CAPTURE",
    "PAT_KO",
    "PAT_KO_TERM",
    "PAT_SINGLE",
    "PAT_LEAVE ",
    "PAT_CMD",
    "PAT_GO_ON_FAIL",
    "PAT_CONSUME",
    NULL,
};

status Match_AddFlagsToStr(MemCtx *m, String *s, word flag){
    status r = READY;
    char *cstr = "";
    boolean first = TRUE;
    if(flag == 0){
        cstr = "PAT_END";
        String_AddBytes(m, s, bytes(cstr), strlen(cstr));
        r |= SUCCESS;
    }
    for(int i = 0; i <= 13; i++){
        if((flag & (1 << i)) != 0){
            if(!first){
                cstr = "|";
                String_AddBytes(m, s, bytes(cstr), strlen(cstr));
            }
            first = FALSE;
            cstr = flagStrs[i];
            String_AddBytes(m, s, bytes(cstr), strlen(cstr));
            r |= SUCCESS;
        }
    }
    if(r == READY){
        r |= NOOP;
    }
    return r;
}

static void match_Reset(Match *mt){
    mt->pat.curDef = mt->pat.startTermDef = mt->pat.startDef;
    mt->remaining = 0;
    mt->counter = 0;
}

static boolean charMatched(word c, PatCharDef *def){
    boolean matched = FALSE;
    matched = (c >= def->from && c <= def->to);

    if((def->flags & PAT_INVERT) != 0 && (def->flags & PAT_KO) == 0){
        matched = !matched;
    }
    return matched;
}

static void match_NextTerm(Match *mt){
    while(
            (mt->pat.curDef < mt->pat.endDef) &&
            (mt->pat.curDef->flags & PAT_TERM) == 0){
        mt->pat.curDef++;
    }
    if((mt->pat.curDef->flags & PAT_TERM) != 0){
        mt->pat.curDef++;
    }
}

static void match_StartOfTerm(Match *mt){
    if(mt->pat.curDef > mt->pat.startDef){
        PatCharDef *prev = mt->pat.curDef;
        prev--;
        while(
                (prev->flags & PAT_TERM) == 0 &&
                (mt->pat.curDef > mt->pat.startDef)){
            if(prev == mt->pat.startDef){
                mt->pat.curDef = mt->pat.startDef;
                break;
            }else{
                mt->pat.curDef--;
            }
            prev--;
        }
    }
}

static void match_EndOfKoTerm(Match *mt){
    while(
            (mt->pat.curDef < mt->pat.endDef) &&
            ((mt->pat.curDef->flags & PAT_KO) != 0) && 
            ((mt->pat.curDef->flags & PAT_KO_TERM) == 0)){
        mt->pat.curDef++;
    }
}

static void match_NextKoTerm(Match *mt){
    match_EndOfKoTerm(mt);
    if((mt->pat.curDef->flags & PAT_KO_TERM) != 0){
        mt->pat.curDef++;
    }
}

static void addCount(MemCtx *m, Match *mt, word flags, int length){
    if(DEBUG_PATMATCH){
        printf("\n\x1b[%dmaddCount:%s%d%s\x1b[0m", 
            DEBUG_PATMATCH,
            ((flags & STRSNIP_GAP) != 0 ? "gap(" : (flags & STRSNIP_UNCLAIMED) != 0 ? "unclaimed(" : ""), 
            length,
            ((flags & STRSNIP_CONTENT) == 0 ? ")" : "")
        );
    }
    if(mt->snip.type.state == ZERO){
        mt->snip.type.state = flags;
    }
    if((mt->snip.type.state & flags) == flags){
        mt->snip.length += length;
    }else{
        String_AddBytes(m, mt->backlog, bytes(&mt->snip), sizeof(StrSnip));
        StrSnip_Init(&mt->snip, flags, mt->snip.start+mt->snip.length, length);
    }
    return;
}

int Match_Total(Match *mt){
    return StrSnipStr_Total(mt->backlog, STRSNIP_CONTENT);
}

status Match_Feed(MemCtx *m, Match *mt, word c){
    if((mt->type.state & NOOP) != 0){

        if(DEBUG_PATMATCH){
            printf("\x1b[%dm'%c' - \x1b[0m", COLOR_RED, c);
            Debug_Print(mt->pat.curDef, TYPE_PATCHARDEF, "NOOP - match_FeedPat: of ", DEBUG_PATMATCH, FALSE);
            printf("\n");
        }

        return mt->type.state;
    }
    boolean matched = FALSE;
    PatCharDef *def;

    if(DEBUG_PATMATCH){
        printf("\n\x1b[%dm%s", DEBUG_PATMATCH, State_ToChars(mt->type.state));
        Debug_Print(mt->pat.curDef, TYPE_PATCHARDEF, "match_FeedPat: of ", DEBUG_PATMATCH, TRUE);
        printf("\n\x1b[1;%dm", DEBUG_PATMATCH);
        if(c == '\n'){
            printf("    '\\n'");
        }else if(c == '\r'){
            printf("    '\\r'");
        }else{
            printf("    '%c'", c);
        }
        printf(": \x1b[0m");
    }

    while(mt->pat.curDef < mt->pat.endDef){
        def = mt->pat.curDef;

        matched = charMatched(c, def);

        if(DEBUG_PATMATCH){
            Match_midDebug('_', c, def, mt, matched, FALSE);
        }

        if((def->flags & PAT_CMD) != 0){
            mt->counter = def->to;
            if((def->from & PAT_GO_ON_FAIL) != 0){
                mt->type.state |= MATCH_GOTO;
            }
            mt->pat.curDef++;
            continue;
        }

        if(matched){
            if((def->flags & (PAT_KO|PAT_INVERT)) == (PAT_KO|PAT_INVERT)){
                mt->type.state |= MATCH_KO_INVERT;
            }else if((def->flags & PAT_KO) != 0){
                if((mt->type.state & MATCH_KO_INVERT) != 0){
                    mt->type.state &= ~MATCH_KO_INVERT;
                    mt->pat.curDef++;
                    continue;
                }else{
                    mt->type.state |= MATCH_KO;
                    if((def->flags & PAT_LEAVE) != 0){
                        mt->type.state |= MATCH_LEAVE;
                    }
                    if((def->flags & PAT_INVERT_CAPTURE) != 0){
                        addCount(m, mt, STRSNIP_UNCLAIMED, 1);
                    }else{
                        addCount(m, mt, STRSNIP_GAP, 1);
                    }

                    match_EndOfKoTerm(mt);
                    PatCharDef *nextDef = mt->pat.curDef+1;

                    if((nextDef->flags & PAT_KO) == 0){
                        if((mt->type.state & MATCH_LEAVE) != 0){
                            goto miss;
                            break;
                        }
                        match_NextTerm(mt);
                        break;
                    }else{
                        break;
                    }
                }
            }
            mt->type.state |= PROCESSING;

            word snipFlag = ZERO;
            if((def->flags & PAT_LEAVE) != 0){
                mt->type.state |= MATCH_INVERTED;
                snipFlag = STRSNIP_UNCLAIMED;
            }else if( (def->flags & (PAT_INVERT_CAPTURE|PAT_INVERT)) == (PAT_INVERT_CAPTURE|PAT_INVERT)){
                /* no increment if it's an invert and no capture */;
                snipFlag = STRSNIP_UNCLAIMED;
            }else if((def->flags & PAT_INVERT_CAPTURE) != 0){
                snipFlag = STRSNIP_GAP;
            }else if((def->flags & PAT_CONSUME) != 0){
                snipFlag = STRSNIP_GAP;
            }else{
                snipFlag = STRSNIP_CONTENT;
            }
            addCount(m, mt, snipFlag, 1);
            
            if((def->flags & (PAT_ANY|PAT_MANY)) != 0 || 
                    ((def->flags & PAT_COUNT) != 0 && (--mt->remaining) > 0)){
                match_StartOfTerm(mt);
                mt->type.state |= MATCH_TERM_FOUND;
            }else{
                match_NextTerm(mt);
                mt->type.state &= ~MATCH_TERM_FOUND;
            }

            break;
        }else{
            if((mt->type.state & MATCH_KO) != 0){
                if((def->flags & PAT_KO) == 0){
                    mt->snip.type.state &= ~NOOP;
                    mt->snip.type.state = STRSNIP_CONTENT;
                    if((mt->type.state & MATCH_LEAVE) != 0){
                        goto miss;
                        break;
                    }
                    mt->type.state &= ~(PROCESSING|MATCH_KO);
                    mt->pat.curDef++;
                    continue;
                }else{
                    match_NextKoTerm(mt);
                    continue;
                }
            }else if((def->flags & (PAT_KO|PAT_OPTIONAL|PAT_ANY)) != 0){
                mt->pat.curDef++;
                continue;
            }else if((def->flags & (PAT_MANY)) != 0){
                if((def->flags & PAT_TERM) != 0){
                    if((mt->type.state & MATCH_TERM_FOUND) != 0){
                        match_NextTerm(mt);
                        if(mt->pat.curDef == mt->pat.endDef){
                            addCount(m, mt, STRSNIP_UNCLAIMED, 1);
                        }
                        continue;
                    }else{
                        goto miss;
                        break;
                    }
                }else{
                    mt->pat.curDef++;
                    continue;
                }
            }
miss:
            mt->type.state &= ~PROCESSING;
            if((mt->type.state & MATCH_SEARCH) != 0){
                match_Reset(mt);
                addCount(m, mt, STRSNIP_GAP, 1);
            }else{
                mt->type.state |= NOOP;
            }
            break;
        }
    }

    if(mt->pat.curDef == mt->pat.endDef){
        String_AddBytes(m, mt->backlog, bytes(&mt->snip), sizeof(StrSnip));
        if(Match_Total(mt) == 0 && (mt->type.state & MATCH_ACCEPT_EMPTY) == 0){
            mt->type.state = NOOP;
        }else{
            mt->type.state = (mt->type.state | SUCCESS) & ~PROCESSING;

            if(DEBUG_MATCH_COMPLETE){
                printf("\x1b[%dmCapture:%d jump:%d ", DEBUG_MATCH_COMPLETE, mt->captureKey, mt->jump);
                Debug_Print(mt, 0, "Match Completed:", DEBUG_MATCH_COMPLETE, TRUE);
                printf("\n");
            }
        }
    }

    if(DEBUG_PATMATCH){
        printf("\n");
        Match_midDebug('_', c, def, mt, matched, TRUE);
        Debug_Print(def, TYPE_PATCHARDEF, " on ", DEBUG_PATMATCH, FALSE);
        printf("\n");
    }

    return mt->type.state;
}

status Match_FeedString(MemCtx *m, Match *mt, String *s, int offset){
    if((mt->type.state & NOOP) != 0){
        return mt->type.state;
    }
    int pos = 0;
    while(s != NULL && s->length < offset){
        offset -= s->length;
        pos += s->length;
        s = String_Next(s);
    }
    while(s != NULL){
        for(int i = offset; i < s->length; i++){
            byte b = s->bytes[i];
            Match_Feed(m, mt, (word)b);
            if((mt->type.state & SUCCESS) != 0){
                break;
            }
        }
        s = String_Next(s); 
        offset = 0;
    }

    return mt->type.state;
}

status Match_FeedEnd(MemCtx *m, Match *mt){
    Match_Feed(m, mt, 0);
    return mt->type.state;
}

status Match_SetString(MemCtx *m, Match *mt, String *s, String *backlog){
    String *ret = PatChar_FromString(m, s);
    if(ret == NULL){
        return ERROR;
    }

    return Match_SetPattern(mt, (PatCharDef *)ret->bytes, backlog); 
}

status Match_SetPattern(Match *mt, PatCharDef *def, String *backlog){
    memset(mt, 0, sizeof(Match));
    mt->type.of = TYPE_PATMATCH;
    mt->pat.startDef = mt->pat.curDef = def;
    int count = 0;
    while(def->flags != PAT_END){
       def++;
       if(++count > PAT_CHAR_MAX_LENGTH){
            Fatal("PatCharDef: PAT_END not found before max", TYPE_PATCHARDEF);
       }
    }
    if(def->flags == PAT_END){
        mt->pat.endDef = def;
    }else{
        Fatal("PatCharDef: end not found", TYPE_PATCHARDEF);
    }
    mt->remaining = -1;
    mt->jump = -1;
    mt->backlog = backlog;
    mt->snip.type.of = TYPE_STRSNIP;

    return SUCCESS;
}
