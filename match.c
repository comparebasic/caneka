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

void Match_Reset(Match *mt){
    mt->position = 0;
    mt->state = READY;
}

status Match_Feed(Match *mt, uchar c){
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
