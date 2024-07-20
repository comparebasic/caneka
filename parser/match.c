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
    return mt;
}

Match *Match_MakePat(MemCtx *m, byte *defs, word npats, word flags){
    Match *mt = (Match *)MemCtx_Alloc(m, sizeof(Match));
    mt->type.of = TYPE_PATMATCH;
    mt->s = String_MakeFixed(m, defs, npats * sizeof(PatCharDef));
    mt->flags = flags;
    return mt;
}

void Match_Reset(Match *mt){
    mt->position = 0;
    mt->state = READY;
}

static status match_FeedPat(Match *mt, byte c){
   if(DEBUG_PATMATCH){
       Debug_Print(mt, TYPE_PATMATCH, "FeedPat: ", DEBUG_PATMATCH, TRUE);
   }

   printf("pos %d\n", mt->position);
   PatCharDef *def = ((PatCharDef *)mt->s->bytes)+(mt->position);  
   i64 length = mt->s->length / sizeof(PatCharDef);
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

            boolean optional = ((def->flags & PAT_ANY) || ((def->flags & PAT_MANY) != 0 && mt->defPosition > 0));
            if((def->flags & PAT_COUNT) != 0){
                matched = (c == def->from);
                total = def->to;
                if(DEBUG_PATMATCH){
                    printf("\x1b[%dm    total %d\x1b[0m\n", DEBUG_PATMATCH, total);
                }
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
                mt->state = PROCESSING;
                if((def->flags & PAT_TERM) != 0){
                    mt->defPosition++;
                    if(total == mt->defPosition){
                        mt->defPosition = 0;
                        mt->position += ocurrences;
                        if(DEBUG_PATMATCH){
                            printf("\x1b[%dm    mt->position %d vs length %ld\x1b[0m\n", DEBUG_PATMATCH, mt->position, length);
                        }
                        if(mt->position == length){
                            mt->defPosition = 0;
                            mt->position = 0;
                            if((def->flags & PAT_MANY) == 0){
                                mt->state = COMPLETE;
                            }
                        }
                    }
                    if(DEBUG_PATMATCH){
                        printf("\x1b[%dm    total %d vs defPosition %d\x1b[0m\n", DEBUG_PATMATCH, total, mt->defPosition);
                    }
                    def++;
                }
            }else if(optional){
                ocurrences++;
                def++;
            }else{
                if((def->flags & PAT_INVERT)){
                    mt->state = INVERTED;
                    mt->defPosition = 0;
                    mt->position = 0;
                    break;
                }else if((def->flags & PAT_ANY) || optional){
                    def++;
                }else{
                    mt->defPosition = 0;
                    mt->state = READY;
                    mt->position = 0;
                    break;
                }
            }
        }
    }

    if(DEBUG_PATMATCH){
        printf("\x1b[%dm  Ret %s\n\n", DEBUG_PATMATCH, State_ToString(mt->state));
    }
    return mt->state;
}

static status match_FeedString(Match *mt, byte c){
    if(DEBUG_MATCH){
       printf("%c of %s %hu", c, mt->s->bytes, mt->s->length);
       Debug_Print(mt, TYPE_MATCH, "FeedPat: ", DEBUG_PATMATCH, TRUE);
    }
    if(mt->s->bytes[mt->position] == c){
        mt->position++;
        if(mt->position == mt->s->length){
            mt->position = 0;
            mt->state = COMPLETE;
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
