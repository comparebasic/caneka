#include <external.h>
#include <caneka.h>

int Match_PatLength(PatCharDef *def){
    int length = 0;
    while(def->flags != 0){
        length++;
        def++;
    }
    return length;
}

Match *Match_Make(MemCtx *m, String *s, word flags){
    Match *mt = (Match *)MemCtx_Alloc(m, sizeof(Match));
    mt->type.of = TYPE_STRINGMATCH;
    mt->flags = flags;
    mt->s = s;
    mt->remaining = -1;
    mt->jump = -1;
    return mt;
}

Match *Match_MakePat(MemCtx *m, byte *defs, word npats, word flags){
    Match *mt = (Match *)MemCtx_Alloc(m, sizeof(Match));
    mt->type.of = TYPE_PATMATCH;
    mt->s = String_MakeFixed(m, defs, npats * sizeof(PatCharDef));
    mt->flags = flags;
    mt->remaining = -1;
    mt->jump = -1;
    return mt;
}

void Match_Reset(Match *mt){
    mt->position = 0;
    mt->state = READY;
}

static status match_FeedPat(Match *mt, byte c){
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
                    printf("\x1b[%dm    matched \%hu'? %d opt %d\x1b[0m\n", DEBUG_PATMATCH, c, matched, optional);
                }else{
                    printf("\x1b[%dm    matched '%c'? %d opt %d\x1b[0m\n", DEBUG_PATMATCH, c, matched, optional);
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

    return mt->state;
}

static status match_FeedString(Match *mt, byte c){
    if(DEBUG_MATCH){
       printf("%c of %s %hu", c, mt->s->bytes, mt->s->length);
       Debug_Print(mt, 0, "FeedPat: ", DEBUG_PATMATCH, TRUE);
    }
    if(mt->s->bytes[mt->position] == c){
        mt->position++;
        if(mt->position == mt->s->length){
            mt->position = 0;
            mt->state = COMPLETE;
            if(DEBUG_ROEBLING_COMPLETE){
                Debug_Print((void *)mt, 0, "Match Complete: ", DEBUG_ROEBLING_COMPLETE, TRUE);
                printf("\n");
            }
        }else{
            mt->state = PROCESSING;
        }
    }else{
        Match_Reset(mt);
    }
    return mt->state;
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
        mt->state = SUCCESS;
    }
    return mt->state;
}
