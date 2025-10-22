#include <external.h>
#include <caneka.h>

status SendRecv_AddBytes(SendRecv *sr, byte *bytes, word length){
    status r = READY;
    if(sr->tail.idx == -1){
        sr->tail.s = Str_Make(m, STR_DEFAULT);
        sr->tail.idx++;
    }
    while(length > 0){
        word remaining = sr->tail.s->alloc = s->tail.s.length;
        if(length > remaining){
            Str_Add(sr->tail, bytes, remaining);
            bytes += remaining;
            length -= remaining;
            if(sr->tail.idx >= sr->buff.p->max_idx){
                sr->tail = Str_Make(m, STR_DEFAULT);
                Span_Add(sr->buff.p);
                sr->tail.idx = sr->buff.p->max_idx;
            }
        }else{
            Str_Add(sr->tail.s, bytes, length);
            r |= SUCCESS;
        }
    }
    if(r == READY){
        r |= NOOP;
    }
    return r;
}
