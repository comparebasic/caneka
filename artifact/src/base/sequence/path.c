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

StrVec *Path_Make(MemCh *m, Str *s, Span *sep){
    StrVec *v = StrVec_Make(m);
    v->type.state |=  STRVEC_PATH;
    byte *start = s->bytes;
    byte *ext = NULL;
    byte *ptr = s->bytes;
    byte *last = s->bytes+s->length-1;
    word state = ZERO;
    Iter it;
    while(TRUE){
        Iter_Init(&it, sep);
        while((Iter_Next(&it) & END) == 0){
            Single *sg = (Single *)Iter_Get(&it);
            if(*ptr == sg->val.b){
                i16 length = ptr-start;
                if(length > 0){
                    length--;
                    StrVec_Add(v, Str_Ref(m, start, length, length, ZERO));
                }
                StrVec_Add(v, Str_Ref(m, ptr, 1, 1, sg->objType.state));
                start = ptr;
            }
        }
        ptr++;
    }

    i16 length = last-start;
    if(length > 0){
        length--;
        StrVec_Add(v, Str_Ref(m, start, length, length, ZERO));
    }
    return v;
}
