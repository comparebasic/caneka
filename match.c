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
   PatCharDef *def = (PatCharDef *)mt->s->bytes+(mt->position * sizeof(PatCharDef));  
   i64 length = mt->s->length / sizeof(PatCharDef);
   boolean matched = FALSE;
   word total;
   if(def->flags == 0){
        mt->state = ERROR;
   }else{
        while(1){
            if((def->flags & PAT_COUNT) != 0){
                matched = (c == def->from);
                total = def->to;
            }else{
                matched = (c <= def->from && c >= def->to);
                total = 1;
            }

            if((def->flags & PAT_INVERT) != 0){
                matched = !matched;
            }

            if(matched){
                mt->state = PROCESSING;
                if((def->flags & PAT_TERM) != 0){
                    if(total == ++(mt->defPosition)){
                        mt->position += mt->defPosition;
                    }
                    if(mt->position == length){
                        mt->state = COMPLETE;
                        break;
                    }
                }
                def++;
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
