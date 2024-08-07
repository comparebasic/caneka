#include <external.h>
#include <caneka.h>

static status match_FeedPat(Match *mt, word c){
    boolean matched = FALSE;
    PatCharDef *def = mt->def.pat+mt->position;
    while(def->flags != PAT_END){
        if(DEBUG_PATMATCH){
            Debug_Print((void *)def, TYPE_PATCHARDEF, "", DEBUG_PATMATCH, FALSE);
            printf(" :");
        }

        /* match the pattern to the char */
        if((def->flags & PAT_ALL) != 0){
            matched = TRUE;
        }else if((def->flags & PAT_COUNT) != 0){
            matched = (c == def->from);
        }else{
            matched = (c >= def->from && c <= def->to);
        }

        if((def->flags & PAT_INVERT) != 0){
            matched = !matched;
        }

        /* handle if matched or not */
        if(matched){
            mt->type.state = PROCESSING;
            mt->count++;

            if(DEBUG_PATMATCH){
                printf("\x1b[%dmY match %d '%c' - pos(%d) %s ", DEBUG_PATMATCH, matched, c, mt->position, State_ToString(mt->type.state));
                Debug_Print((void *)def, TYPE_PATCHARDEF, "", DEBUG_PATMATCH, FALSE);
                printf("\n");
            }
            
            /* if it's a many or any match rewind to the first non PAT_TERM def */
            if((def->flags & (PAT_MANY|PAT_ANY)) != 0){
                while((mt->position-1) > 0 && ((def+(mt->position-1))->flags & PAT_TERM) == 0){
                    mt->position--;
                }
            }else{
                /* if  we have matched in the middle of the term, fast fowrard to the end of it */
                while(((mt->def.pat+(mt->position))->flags & PAT_TERM) != 0 && (mt->position+1) < mt->length){
                    mt->position++;
                }
                mt->position++;
            }

            break;
        }else{
            /* only knockout the status if the character is not optional */
            if((def->flags & (PAT_OPTIONAL|PAT_ANY|PAT_MANY)) == 0){
                mt->type.state = READY;

                if(DEBUG_PATMATCH){
                    printf("\x1b[%dmN match %d '%c' - pos(%d) %s ", DEBUG_PATMATCH, matched, c, mt->position, State_ToString(mt->type.state));
                    Debug_Print((void *)def, TYPE_PATCHARDEF, "", DEBUG_PATMATCH, FALSE);
                    printf("\n");
                }

                mt->position = 0;

                break;
            }

            if(DEBUG_PATMATCH){
                printf("\x1b[%dmO match %d '%c' - pos(%d) %s ", DEBUG_PATMATCH, matched, c, mt->position, State_ToString(mt->type.state));
                Debug_Print((void *)def, TYPE_PATCHARDEF, "", DEBUG_PATMATCH, FALSE);
                printf("\n");
            }

            mt->position++;
        }

        def = mt->def.pat+mt->position;
    }

    /* after the loop has broken, if we are in a PROCESSING state and have reached the end of the pattern, we are COMPLETE */
    if((mt->type.state & PROCESSING) != 0){
        if(mt->position == mt->length){
            mt->type.state |= SUCCESS;
        }
    }

    return mt->type.state;
}

static status match_FeedString(Match *mt, byte c){
    if(DEBUG_MATCH){
       printf("%c of %s %hu", c, mt->def.s->bytes, mt->def.s->length);
       Debug_Print(mt, 0, "FeedPat: ", DEBUG_PATMATCH, TRUE);
    }
    if(mt->def.s->bytes[mt->position] == c){
        mt->position++;
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
        mt->type.state = READY;
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

status Match_SetPattern(Match *mt, PatCharDef def[]){
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
