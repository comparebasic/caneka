#include "external.h"
#include "filestore.h"

Match *Match_Make(MemCtx *m, String *s, int anchor, int intval){
    Match *mt = (Match *)MemCtx_Alloc(m, sizeof(Match));
    mt->type = TYPE_STRINGMATCH;
    mt->s = s;
    mt->anchor = anchor;
    mt->intval = intval;
    return mt;
}

Match *Match_MakePat(MemCtx *m, byte *defs, word npats,  int anchor, int intval){
    Match *mt = (Match *)MemCtx_Alloc(m, sizeof(Match));
    mt->type = TYPE_PATMATCH;
    mt->s = String_MakeFixed(m, defs, npats * sizeof(PatCharDef));
    mt->anchor = anchor;
    mt->intval = intval;
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

   PatCharDef *def = ((PatCharDef *)mt->s->bytes)+(mt->position);  
   i64 length = mt->s->length / sizeof(PatCharDef);
   boolean matched = FALSE;
   word total;
   if(def->flags == 0){
        mt->state = ERROR;
   }else{
        while(1){
            if(DEBUG_PATMATCH){
                Debug_Print(def, TYPE_PATCHARDEF, "  While: ", DEBUG_PATMATCH, TRUE);
                printf("\n");
            }
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
                printf("\x1b[%dm    matched %c? %d\x1b[0m\n", DEBUG_PATMATCH, c, matched);
            }
            if(matched){
                mt->state = PROCESSING;
                if((def->flags & PAT_TERM) != 0){
                    mt->defPosition++;
                    if(DEBUG_PATMATCH){
                        printf("\x1b[%dm    total %d vs defPosition %d\x1b[0m\n", DEBUG_PATMATCH, total, mt->defPosition);
                    }
                    if(total == mt->defPosition){
                        mt->defPosition = 0;
                        mt->position++;
                        if(mt->position == length){
                            mt->state = COMPLETE;
                            mt->defPosition = 0;
                        }
                    }
                    break;
                }
            }else{
                if((def->flags & PAT_ANY) || ((def->flags & PAT_MANY) && mt->defPosition > 0)){
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
    if(mt->s->bytes[mt->position] == c){
        mt->position++;
        if(mt->position == mt->s->length){
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
    if(mt->type == TYPE_PATMATCH){
        return match_FeedPat(mt, c);
    }else{
        return match_FeedString(mt, c);
    }
}
