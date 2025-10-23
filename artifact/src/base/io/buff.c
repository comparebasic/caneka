#include <external.h>
#include <caneka.h>

status Buff_AddBytes(Buff *bf, byte *bytes, word length){
    status r = READY;
    if(bf->tail.idx == -1){
        bf->tail.s = Str_Make(bf->m, STR_DEFAULT);
        Span_Add(bf->v->p, (Abstract *)bf->tail.s);
        bf->tail.idx = bf->v->p->max_idx;
    }
    word remaining = bf->tail.s->alloc - bf->tail.s->length;
    while(length > 0){
        if(length > remaining){
            Str_Add(bf->tail.s, bytes, remaining);
            bf->unsent.total += remaining;
            bf->v->total += remaining;
            bytes += remaining;
            length -= remaining;
            if(bf->tail.idx >= bf->v->p->max_idx){
                bf->tail.s = Str_Make(bf->m, STR_DEFAULT);
                Span_Add(bf->v->p, (Abstract *)bf->tail.s);
                bf->tail.idx = bf->v->p->max_idx;
            }
        }else{
            Str_Add(bf->tail.s, bytes, length);
            bf->unsent.total += length;
            bf->v->total += length;
            length -= length;
            r |= SUCCESS;
        }
    }
    if(r == READY){
        r |= NOOP;
    }
    return r;
}

Buff *Buff_Make(MemCh *m, word flags){
    Buff *bf = (Buff *)MemCh_AllocOf(m, sizeof(Buff), TYPE_BUFF);
    bf->type.of = TYPE_BUFF;
    bf->type.state = flags;
    bf->m = m;
    bf->fd = -1;
    bf->v = StrVec_Make(m);
    bf->tail.idx = -1;
    return bf;
}
