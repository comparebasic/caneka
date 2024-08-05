#include <external.h>
#include <caneka.h>

/*
   if(DEBUG_PATMATCH){
       if(c == '\r'){
           printf("\x1b[%dm'\\r' -> ", DEBUG_PATMATCH);
       }else if(c == '\n'){
           printf("\x1b[%dm'\\n' -> ", DEBUG_PATMATCH);
       }else{
           printf("\x1b[%dm'%c' -> ", DEBUG_PATMATCH, c);
       }
       Debug_Print(mt, TYPE_PATMATCH, "FeedPat: ", DEBUG_PATMATCH, TRUE);
   }

   PatCharDef *def = ((PatCharDef *)mt->s->bytes)+(mt->position);  
   int length = mt->s->length / sizeof(PatCharDef);
   boolean matched = FALSE;
   word total;
   int ocurrences = 1;
   if(def->flags == 0){
        mt->state = ERROR;
   }else{
        while(1){
            if(def->flags == 0){
                break;
            }

            if(DEBUG_PATMATCH){
                Debug_Print(def, TYPE_PATCHARDEF, "  While: ", DEBUG_PATMATCH, TRUE);
                printf("\n");
            }

            boolean optional = ((def->flags & PAT_ANY|PAT_MANY) != 0 && mt->defPosition > 0);
            if((def->flags & PAT_ALL) != 0){
                matched = TRUE;
                total = 1;
            }else if((def->flags & PAT_COUNT) != 0){
                matched = (c == def->from);
                total = def->to;
            }else{
                matched = (c >= def->from && c <= def->to);
                total = 1;
            }

            if((def->flags & PAT_INVERT) != 0){
                matched = !matched;
            }

            if(DEBUG_PATMATCH){
                if(c == '\r' || c == '\n' || c == '\t'){
                    printf("\x1b[%dm    matched at %u \%hu'? %d opt %d\x1b[0m\n", DEBUG_PATMATCH, mt->position, c, matched, optional);
                }else{
                    printf("\x1b[%dm    matched at %u '%c'? %d opt %d\x1b[0m\n", DEBUG_PATMATCH, mt->position, c, matched, optional);
                }
            }

            if(matched){
                if(DEBUG_PATMATCH){
                    Debug_Print((void *)def, TYPE_PATCHARDEF, "matched ", COLOR_RED, TRUE);
                    printf(" - %d\n", def->flags & PAT_INVERT);
                }
                mt->state = PROCESSING;
                mt->defPosition++;
                if(total == mt->defPosition){
                    mt->defPosition = 0;
                    mt->position += ocurrences;
                    if(DEBUG_PATMATCH){
                        printf("\x1b[%dm    mt->position %d vs length %d\x1b[0m\n", DEBUG_PATMATCH, mt->position, length);
                    }

                    while((def->flags & (PAT_TERM|PAT_INVERT)) == 0){
                        def++;
                        mt->position++;
                    }

                    if(mt->position == length){
                        mt->defPosition = 0;
                        if((def->flags & PAT_MANY) == 0){
                            mt->state = COMPLETE;
                            if(DEBUG_PATMATCH){
                                Debug_Print((void *)mt, 0, "Match Complete (matched): ", DEBUG_PATMATCH, TRUE);
                                printf("\n");
                            }
                        }else{
                            mt->position = 0;
                        }

                    }
                    if((def->flags & PAT_TERM) != 0){
                        mt->count++;
                        if(mt->remaining >= 0 && mt->count == mt->remaining){
                            mt->state = COMPLETE;
                            if(DEBUG_PATMATCH){
                                Debug_Print((void *)mt, 0, "Match Complete (matched): ", DEBUG_PATMATCH, TRUE);
                                printf("\n");
                            }
                        }
                    }
                }
                if(DEBUG_PATMATCH){
                    printf("\x1b[%dm    total %d vs defPosition %d\x1b[0m\n", DEBUG_PATMATCH, total, mt->defPosition);
                }
                if((def->flags & PAT_INVERT) != 0){
                    def++;
                }else{
                    if(mt->position > 0){
                        while(mt->position > 0 && ((def-1)->flags & PAT_TERM) == 0){
                            def--;
                            mt->position--;
                        }
                    }
                    break;
                }
            }else if(optional){
                ocurrences++;
                def++;
            }else{
                if((def->flags & PAT_INVERT)){
                    mt->state = INVERTED;
                    if((def->flags & PAT_IGNORE) != 0){
                        mt->state |= PROCESSING;
                    }
                    mt->defPosition = 0;
                    mt->position--;
                    if(DEBUG_PATMATCH){
                        Debug_Print((void *)mt, 0, "Match Complete (invert): ", DEBUG_PATMATCH, TRUE);
                        printf("\n");
                    }
                    break;
                }else if(optional){
                    def++;
                }else{
                    if((def->flags & PAT_TERM) != 0 && ((def->flags & PAT_ANY) != 0 && mt->position > 0)){
                        mt->state = INVERTED;
                        break;
                    }else if((def->flags & PAT_TERM) != 0){
                        mt->defPosition = 0;
                        mt->state = READY;
                        mt->position = 0;
                        break;
                    }else{
                        ocurrences++;
                        def++;
                    }
                }
            }
        }
    }

    if(mt->state == READY && length == 0){
        mt->state = SUCCESS;
    }

    if(DEBUG_PATMATCH){
        printf("\x1b[%dm  Ret %s\n\n", DEBUG_PATMATCH, State_ToString(mt->state));
    }

    return mt->type.state;
}
*/

static status match_FeedPat(Match *mt, word c){
    boolean matched = FALSE;
    PatCharDef *def = mt->def.pat+mt->position;
    while(def->flags != PAT_END){
        if(DEBUG_MATCH){
            Debug_Print((void *)def, TYPE_PATCHARDEF, "", DEBUG_MATCH, FALSE);
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

            if(DEBUG_MATCH){
                printf("\x1b[%dmY match %d '%c' - pos(%d) %s ", COLOR_YELLOW, matched, c, mt->position, State_ToString(mt->type.state));
                Debug_Print((void *)def, TYPE_PATCHARDEF, "", COLOR_YELLOW, FALSE);
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

                if(DEBUG_MATCH){
                    printf("\x1b[%dmN match %d '%c' - pos(%d) %s ", COLOR_YELLOW, matched, c, mt->position, State_ToString(mt->type.state));
                    Debug_Print((void *)def, TYPE_PATCHARDEF, "", COLOR_YELLOW, FALSE);
                    printf("\n");
                }

                mt->position = 0;

                break;
            }

            if(DEBUG_MATCH){
                printf("\x1b[%dmO match %d '%c' - pos(%d) %s ", COLOR_YELLOW, matched, c, mt->position, State_ToString(mt->type.state));
                Debug_Print((void *)def, TYPE_PATCHARDEF, "", COLOR_YELLOW, FALSE);
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
