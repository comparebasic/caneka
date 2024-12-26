#include <external.h>
#include <caneka.h>

static void match_Reset(Match *mt){
    mt->pat.curDef = mt->pat.startTermDef = mt->pat.startDef;
    mt->count = 0;
    mt->lead = 0;
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
            !HasFlag(mt->pat.curDef->flags, PAT_TERM)){
        mt->pat.curDef++;
    }
    if(HasFlag(mt->pat.curDef->flags, PAT_TERM)){
        mt->pat.curDef++;
    }
}

static void match_StartOfTerm(Match *mt){
    if(mt->pat.curDef > mt->pat.startDef){
        PatCharDef *prev = mt->pat.curDef;
        prev--;
        while(
                !HasFlag(prev->flags, PAT_TERM) &&
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

status Match_Feed(Match *mt, word c){
    if(HasFlag(mt->type.state, NOOP)){

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
        Debug_Print(mt->pat.curDef, TYPE_PATCHARDEF, "\nmatch_FeedPat: of ", DEBUG_PATMATCH, TRUE);
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
            if(HasFlag(def->from, PAT_GO_ON_FAIL)){
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
                    if((def->flags & PAT_INVERT_CAPTURE) == 0){
                        mt->tail++;
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

            if(HasFlag(def->flags, PAT_LEAVE)){
                mt->type.state |= MATCH_INVERTED;
            }else if((def->flags & PAT_INVERT_CAPTURE) != 0){
                if(mt->count == 0){
                    mt->lead++;
                }else {
                    mt->tail++;
                }
            }else{
                mt->count = mt->count + mt->tail + 1;
                mt->tail = 0;
            }
            
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
                    mt->type.state &= ~MATCH_KO;
                    if((mt->type.state & MATCH_LEAVE) != 0){
                        goto miss;
                        break;
                    }
                    mt->type.state &= ~PROCESSING;
                    mt->pat.curDef++;
                    continue;
                }else{
                    match_NextKoTerm(mt);
                    continue;
                }
            }else if((def->flags & (PAT_KO|PAT_OPTIONAL)) != 0){
                mt->pat.curDef++;
                continue;
            }else if((def->flags & (PAT_MANY)) != 0){
                if(HasFlag(def->flags, PAT_TERM)){
                    if((mt->type.state & MATCH_TERM_FOUND) != 0){
                        match_NextTerm(mt);
                        continue;
                    }else{
                        goto miss;
                        break;
                    }
                }else{
                    mt->pat.curDef++;
                    continue;
                }
            }else if((def->flags & (PAT_ANY)) != 0){
                if(HasFlag(def->flags, PAT_TERM) && !HasFlag(mt->type.state, MATCH_TERM_FOUND)){
                    match_NextTerm(mt);
                    break;
                }else{
                    mt->pat.curDef++;
                    continue;
                }
            }
miss:
            mt->type.state &= ~PROCESSING;
            if(HasFlag(mt->type.state, SEARCH)){
                match_Reset(mt);
            }else{
                mt->type.state |= NOOP;
            }
            break;
        }
    }

    if(mt->pat.curDef == mt->pat.endDef){
        if(Match_Total(mt) == 0){
            mt->type.state = NOOP;
        }else{
            mt->type.state = (mt->type.state | SUCCESS) & ~PROCESSING;

            if(DEBUG_MATCH_COMPLETE){
                Debug_Print(mt, 0, "Match Completed:%s", DEBUG_MATCH_COMPLETE, TRUE);
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

status Match_FeedEnd(Match *mt){
    if(mt->remaining == 0){
        mt->type.state = SUCCESS;
    }
    return mt->type.state;
}

status Match_SetString(MemCtx *m, Match *mt, String *s){
    String *ret = PatChar_FromString(m, s);
    if(ret == NULL){
        return ERROR;
    }

    return Match_SetPattern(mt, (PatCharDef *)ret->bytes); 
}

status Match_SetPattern(Match *mt, PatCharDef *def){
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


    return SUCCESS;
}
