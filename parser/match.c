#include <external.h>
#include <caneka.h>

static void match_Reset(Match *mt){
    if(mt->type.of == TYPE_PATMATCH){
        mt->def.pat.curDef = mt->def.pat.startTermDef = mt->def.pat.startDef;
    }else if (mt->type.of == TYPE_STRINGMATCH){
        mt->def.str.position = 0;
    }
    mt->count = 0;
    mt->lead = 0;
}

static boolean charMatched(word c, PatCharDef *def){
    boolean matched = FALSE;
    if((def->flags & PAT_ALL) != 0){
        matched =  TRUE;
    }else if((def->flags & PAT_COUNT) != 0){
        matched = (c == def->from);
    }else{
        matched = (c >= def->from && c <= def->to);
    }

    if((def->flags & PAT_INVERT) != 0){
        matched = !matched;
    }
    return matched;
}

static void match_NextTerm(Match *mt){
    while(
            (mt->def.pat.curDef < mt->def.pat.endDef) &&
            !HasFlag(mt->def.pat.curDef->flags, PAT_TERM)){
        mt->def.pat.curDef++;
    }
    if(HasFlag(mt->def.pat.curDef->flags, PAT_TERM)){
        mt->def.pat.curDef++;
    }
}

static void match_StartOfTerm(Match *mt){
    if(mt->def.pat.curDef > mt->def.pat.startDef){
        PatCharDef *prev = mt->def.pat.curDef;
        prev--;
        while(
                !HasFlag(prev->flags, PAT_TERM) &&
                (mt->def.pat.curDef > mt->def.pat.startDef)){
            if(prev == mt->def.pat.startDef){
                mt->def.pat.curDef = mt->def.pat.startDef;
                break;
            }else{
                mt->def.pat.curDef--;
            }
            prev--;
        }
    }
}

static status match_FeedPat(Match *mt, word c){
    if(HasFlag(mt->type.state, MISS)){
        if(DEBUG_PATMATCH){
            Debug_Print(mt->def.pat.curDef, TYPE_PATCHARDEF, "\nmatch_FeedPat: of ", DEBUG_PATMATCH, TRUE);
            printf(" - MISS\n");
        }
        return mt->type.state;
    }
    boolean matched = FALSE;
    PatCharDef *def;
    if(DEBUG_PATMATCH){
        Debug_Print(mt->def.pat.curDef, TYPE_PATCHARDEF, "\nmatch_FeedPat: of ", DEBUG_PATMATCH, TRUE);
        printf("\n");
    }
    while(mt->def.pat.curDef < mt->def.pat.endDef){
        def = mt->def.pat.curDef;
        matched = charMatched(c, def);

        if(DEBUG_PATMATCH){
            Match_midDebug('_', c, def, mt, matched);
        }
        if(matched){
            if(HasFlag(def->flags, PAT_KO)){
                if(!HasFlag(def->flags, PAT_NO_CAPTURE) || HasFlag(def->flags, PAT_CONSUME)){
                    mt->tail++;
                }
                if(HasFlag(def->flags, PAT_SET_NOOP) || mt->count == 0){
                    mt->type.state |= MISS;
                    mt->def.pat.curDef = mt->def.pat.startDef;
                    return mt->type.state;
                }
                mt->type.state &= ~PROCESSING;
                mt->type.state |= KO;
                match_NextTerm(mt);
                break;
            }
            mt->type.state |= PROCESSING;

            if(HasFlag(def->flags, PAT_NO_CAPTURE)){
                if(mt->count == 0){
                    mt->lead++;
                }else if((def+1) == mt->def.pat.endDef){
                    mt->tail++;
                }else{
                    mt->count++;
                }
            }else{
                mt->count++;
            }

            if((def->flags & (PAT_ANY|PAT_MANY)) != 0){
                match_StartOfTerm(mt);
                mt->type.state |= TERM_FOUND;
            }else{
                match_NextTerm(mt);
                mt->type.state &= ~TERM_FOUND;
            }
            break;
        }else{
            if((def->flags & (PAT_KO|PAT_OPTIONAL)) != 0){
                printf("KO def++\n");
                mt->def.pat.curDef++;
                continue;
            }else if((def->flags & (PAT_MANY|PAT_ANY)) != 0){
                if(HasFlag(def->flags, PAT_TERM) && !HasFlag(mt->type.state, TERM_FOUND)){
                    mt->type.state &= ~PROCESSING;
                    match_Reset(mt);
                    break;
                }else{
                    mt->def.pat.curDef++;
                    continue;
                }
            }else{
                mt->type.state &= ~PROCESSING;
                match_Reset(mt);
                break;
            }
        }
    }

    if(DEBUG_PATMATCH){
        Match_midDebug('E', c, def, mt, charMatched(c, mt->def.pat.curDef));
    }

    if(mt->def.pat.curDef == mt->def.pat.endDef){
        mt->type.state = (mt->type.state | COMPLETE) & ~PROCESSING;
        if(DEBUG_MATCH_COMPLETE){
            Debug_Print(mt, 0, "Match Completed:%s", DEBUG_MATCH_COMPLETE, TRUE);
            printf("\n");
        }
    }

    if(DEBUG_PATMATCH){
        printf("\x1b[%dmround: <%s> on \x1b[0m", DEBUG_PATMATCH, State_ToString(mt->type.state));
        Debug_Print(def, TYPE_PATCHARDEF, "", DEBUG_PATMATCH, FALSE);
        printf("\n");
    }

    return mt->type.state;
}

static status match_FeedString(Match *mt, byte c){
    if(DEBUG_MATCH){
       printf("\x1b[%dm%c of %s %u", DEBUG_MATCH, c, mt->def.str.s->bytes, mt->def.str.s->length);
       Debug_Print(mt, 0, " FeedPat: ", DEBUG_MATCH, TRUE);
       printf("\n");
    }
    if(mt->def.str.s->bytes[mt->def.str.position] == c){
        mt->def.str.position++;
        mt->count++;
        if(mt->def.str.position == mt->def.str.s->length){
            mt->def.str.position = 0;
            mt->type.state = COMPLETE;
            if(DEBUG_ROEBLING_COMPLETE){
                Debug_Print((void *)mt, 0, "Match Complete: ", DEBUG_ROEBLING_COMPLETE, TRUE);
                printf("\n");
            }
        }else{
            mt->type.state = PROCESSING;
        }
    }else{
        mt->def.str.position = 0;
        mt->type.state = MISS;
    }
    return mt->type.state;
}

status Match_Feed(Match *mt, byte c){
    if(DEBUG_ROEBLING_CONTENT){
        if(c == '\r'){
            printf("\x1b[%dm\\r\x1b[0m", DEBUG_ROEBLING_CONTENT);
        }else if(c == '\n'){
            printf("\x1b[%dm\\n\x1b[0m", DEBUG_ROEBLING_CONTENT);
        }else{
            printf("\x1b[%dm%c\x1b[0m", DEBUG_ROEBLING_CONTENT, c);
        }
    }
    if(mt->type.of == TYPE_PATMATCH){
        return match_FeedPat(mt, c);
    }else{
        return match_FeedString(mt, c);
    }
}

status Match_FeedEnd(Match *mt){
    if(mt->remaining == 0){
        mt->type.state = SUCCESS;
    }
    return mt->type.state;
}

status Match_SetPattern(Match *mt, PatCharDef *def){
    memset(mt, 0, sizeof(Match));
    mt->type.of = TYPE_PATMATCH;
    mt->def.pat.startDef = mt->def.pat.curDef = def;
    int count = 0;
    while(def->flags != PAT_END){
       def++;
       if(++count > PAT_CHAR_MAX_LENGTH){
            Fatal("PatCharDef: PAT_END not found before max", TYPE_PATCHARDEF);
       }
    }
    if(def->flags == PAT_END){
        mt->def.pat.endDef = def;
    }else{
        Fatal("PatCharDef: end not found", TYPE_PATCHARDEF);
    }
    mt->remaining = -1;
    mt->jump = -1;

    return SUCCESS;
}

status Match_SetString(Match *mt, String *s){
    memset(mt, 0, sizeof(Match));
    mt->type.of = TYPE_STRINGMATCH;
    mt->def.str.s = s;
    mt->remaining = -1;
    mt->jump = -1;

    return SUCCESS;
}
