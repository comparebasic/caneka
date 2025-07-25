#include <external.h>
#include <caneka.h>

status Path_RangeOf(MemCh *m, StrVec *path, word sep, Coord *cr){
    status r = READY;
    cr->a = cr->b = -1;
    Iter it;
    Iter_Init(&it, path->p);
    while((Iter_Next(&it) & END) == 0){
        Str *s = (Str *)Iter_Get(&it);
        if(s->type.state & sep){
            if(cr->a == -1){
                cr->a = it.idx;
                r |= SUCCESS;
            }
            cr->b = it.idx;
        }
    }

    if(r == READY){
        r |= NOOP;
    }

    return r;
}

status Path_Around(MemCh *m, StrVec *path, word sep, Coord *cr){
    status r = READY;
    cr->a = cr->b = -1;
    Iter it;
    Iter_Init(&it, path->p);
    while((Iter_Next(&it) & END) == 0){
        Str *s = (Str *)Iter_Get(&it);
        if(s->type.state & sep){
            if(it.idx > 0 && it.idx < it.p->max_idx){
                cr->a = it.idx-1;
                cr->b = it.idx+1;
                r |= SUCCESS;
                break;
            }
        }
    }

    if(r == READY){
        r |= NOOP;
    }

    return r;
}

status Path_Annotate(MemCh *m, StrVec *v, Span *sep){
    status r = READY;

    Span *p = Span_Clone(m, v->p);
    Span_Wipe(v->p);

    Iter it;
    Iter sepIt;
    Iter_Init(&sepIt, sep);
    Iter_Init(&it, p);
    while((Iter_Next(&it) & END) == 0){
        Str *s = Iter_Get(&it);
        byte *start = s->bytes;
        byte *ptr = s->bytes;
        byte *last = s->bytes+s->length-1;
        while(TRUE){
            Iter_Init(&sepIt, sep);
            while((Iter_Next(&sepIt) & END) == 0){
                Single *sg = (Single *)Iter_Get(&sepIt);
                if(*ptr == sg->val.b){
                    i16 length = ptr-start;
                    if(length > 0){
                        Str *s = Str_Ref(m, start, length, length, ZERO);
                        StrVec_Add(v, s);
                    }
                    StrVec_Add(v, Str_Ref(m, ptr, 1, 1, sg->objType.state));
                    start = ptr;
                    if(start < last){
                        start++; 
                    }
                    r |= SUCCESS;
                }
            }
            if(ptr == last){
                break;
            }
            ptr++;
        }
        i16 length = last-start;
        if(length > 0){
            length++;
            StrVec_Add(v, Str_Ref(m, start, length, length, ZERO));
        }
    }

    if(r == READY){
        r |= NOOP;
    }else{
        v->type.state |= STRVEC_PATH;
    }

    return r;
}
