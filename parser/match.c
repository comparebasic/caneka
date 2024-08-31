#include <external.h>
#include <caneka.h>

static void debug(char type, word c, PatCharDef *def, Match *mt, boolean matched){
    if(c == '\n'){
        printf("\x1b[%dm  %c match %d \x1b[0;1m'\\n'\x1b[0;%dm - pos(%d) count(%d) %s ", DEBUG_PATMATCH, type, matched,
            DEBUG_PATMATCH, mt->position, mt->count, State_ToString(mt->type.state));
    }else if(c == '\r'){
        printf("\x1b[%dm  %c match %d \x1b[0;1m'\\r'\x1b[0;%dm - pos(%d) count(%d) %s ", DEBUG_PATMATCH, type, matched,
            DEBUG_PATMATCH, mt->position, mt->count, State_ToString(mt->type.state));
    }else{
        printf("\x1b[%dm  %c match %d \x1b[0;1m'%c'\x1b[0;%dm - pos(%d) count(%d) %s ", DEBUG_PATMATCH, type,  matched,
            c, DEBUG_PATMATCH, mt->position, mt->count, State_ToString(mt->type.state));
    }
    Debug_Print((void *)def, TYPE_PATCHARDEF, "", DEBUG_PATMATCH, FALSE);
    printf("\n");
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

static void incrCount(Match *mt, PatCharDef *def){
    if(HasFlag(def->flags, PAT_IGNORE)){
        if(mt->count == 0){
            mt->lead++;
        }else{
            mt->type.state |= OPTIONAL;
        }
    }else if(!HasFlag(def->flags, PAT_IGNORE)){
        mt->type.state &= ~OPTIONAL;
        mt->count++;
    }
}

static boolean reposition(Match *mt, PatCharDef *def){
    if(HasFlag(def->flags, PAT_OR)){
        mt->position++;
        return FALSE;
    }else if((def->flags & (PAT_MANY|PAT_ANY)) != 0){
        /* if it's a many or any match rewind to the first non PAT_TERM def */
        while((mt->position-1) > 0 && ((def+(mt->position-1))->flags & PAT_TERM) == 0){
            mt->position--;
        }
    }else{
        /* if  we have matched in the middle of the term, fast fowrard to the end of it */
        while(((mt->def.pat+(mt->position))->flags & PAT_TERM) == 0 && (mt->position+1) < mt->length){
            mt->position++;
        }
        mt->position++;
    }
    return TRUE;
}

static void reset(Match *mt){
    mt->type.state &= ~(PROCESSING|COMPLETE);
    mt->position = 0;
}

static status match_FeedPat(Match *mt, word c){
    boolean matched = FALSE;
    PatCharDef *def = mt->def.pat+mt->position;
    if(DEBUG_PATMATCH){
        Debug_Print(def, TYPE_PATCHARDEF, ">", DEBUG_PATMATCH, TRUE);
        printf("\n");
    }
    while(def->flags != PAT_END){
        if(DEBUG_PATMATCH){
            debug('_', c, def, mt, matched);
        }

        if((def->flags & (PAT_MANY|PAT_ANY)) != 0){
            mt->type.state |= ELASTIC;
        }else if(def->flags & PAT_TERM){
            mt->type.state &= ~ELASTIC;
        }

        if(charMatched(c, def)){
            mt->type.state |= PROCESSING;
            incrCount(mt, def);
            if(reposition(mt, def)){
                break;
            }
        }else{
            if(HasFlag(def->flags, PAT_TERM) && !HasFlag(mt->type.state, PROCESSING)){
                reset(mt);
                break;
            }
            if(HasFlag(def->flags, (PAT_MANY|PAT_INVERT))){
                if(mt->count > 0){
                    mt->type.state = (COMPLETE|OPTIONAL);
                    break;
                }
            }
            if(HasFlag(def->flags, PAT_MANY)){
                if(mt->count > 0){
                    mt->type.state = (COMPLETE|OPTIONAL);
                    break;
                }
            }
            if((def->flags & (PAT_OPTIONAL|PAT_ANY)) == 0){
                break;
            }
            mt->position++;
        }

        def = mt->def.pat+mt->position;
    }

    /* after the loop has broken, if we are in a PROCESSING state and have reached the end of the pattern, we are COMPLETE */
    if(HasFlag(mt->type.state, PROCESSING)){
        if(mt->position == mt->length){
            if(!HasFlag(mt->type.state, ELASTIC)){
                mt->type.state |= SUCCESS;
            }
        }
    }

    if(DEBUG_PATMATCH){
        printf("\x1b[%dm <%s\x1b[0m\n", DEBUG_PATMATCH, State_ToString(mt->type.state));
    }
    return mt->type.state;
}

static status match_FeedString(Match *mt, byte c){
    if(DEBUG_MATCH){
       printf("\x1b[%dm%c of %s %u", DEBUG_PATMATCH, c, mt->def.s->bytes, mt->def.s->length);
       Debug_Print(mt, 0, "FeedPat: ", DEBUG_PATMATCH, TRUE);
       printf("\n");
    }
    if(mt->def.s->bytes[mt->position] == c){
        mt->position++;
        mt->count++;
        if(mt->position == mt->def.s->length){
            mt->position = 0;
            mt->type.state = COMPLETE;
            if(DEBUG_ROEBLING_COMPLETE){
                Debug_Print((void *)mt, 0, "Match Complete: ", DEBUG_ROEBLING_COMPLETE, TRUE);
                printf("\n");
            }
        }else{
            mt->type.state = PROCESSING;
        }
    }else{
        mt->position = 0;
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
    mt->def.pat = def;
    while(def->flags != PAT_END){
       mt->length++;
       def++;
    }
    mt->remaining = -1;
    mt->jump = -1;

    return SUCCESS;
}

status Match_SetString(Match *mt, String *s){
    memset(mt, 0, sizeof(Match));
    mt->type.of = TYPE_STRINGMATCH;
    mt->def.s = s;
    mt->length = s->length;
    mt->remaining = -1;
    mt->jump = -1;

    return SUCCESS;
}
