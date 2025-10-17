#include <external.h>
#include <caneka.h>

status Path_AddSlash(MemCh *m, StrVec *path){
    Str *s = Span_Get(path->p, path->p->max_idx);
    if(s->bytes[s->length-1] != '/'){
        Str_Add(s, (byte *)"/", 1); 
    }
    return s->type.state;
}

StrVec *Path_SubClone(MemCh *m, StrVec *path, i32 count){
    StrVec *v = StrVec_Make(m);

    Iter it;
    Iter_Init(&it, path->p);
    while((Iter_Next(&it) & END) == 0){
        Str *s = (Str *)Iter_Get(&it);
        if(it.idx >= count){
            StrVec_Add(v, s);
        }
    }

    return v;
}

status Path_AddStr(StrVec *path, Str *add){
    Str *s = Span_Get(path->p, path->p->max_idx);
    if(s == NULL || s->bytes[s->length-1] != '/'){
        StrVec_Add(path, Str_Ref(path->p->m, (byte *)"/", 1, 2, MORE));
    }
    return StrVec_Add(path, add);
}

status Path_Add(MemCh *m, StrVec *path, StrVec *add){
    Str *s = Span_Get(path->p, path->p->max_idx);
    Str *sa = Span_Get(add->p, 0);
    if(
            (s == NULL || s->bytes[s->length-1] != '/') && 
            (sa == NULL || sa->bytes[0] != '/')
        ){
        StrVec_Add(path, Str_Ref(m, (byte *)"/", 1, 2, MORE));
    }
    if((add->type.state & STRVEC_PATH) == 0){
        path->type.state |= ERROR;
    }
    return StrVec_AddVec(path, add);
}

Str *Path_StrAdd(MemCh *m, StrVec *path, Str *seg){
    Str *s = StrVec_ToStr(m, path, STR_DEFAULT);
    if(s->bytes[s->length-1] != '/'){
        Str_Add(s, (byte *)"/", 1); 
    }
    Str_Add(s, seg->bytes, seg->length);
    if((s->type.state & ERROR) == 0){
        return s;
    }
    return NULL;
}

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

StrVec *Path_Base(MemCh *m, StrVec *path){
    StrVec *v = StrVec_Make(m);
    Iter it;
    Iter_Init(&it, path->p);
    while((Iter_Next(&it) & END) == 0){
        Str *s = (Str *)Iter_Get(&it);
        if(s->type.state & LAST){
            break;
        }
        StrVec_Add(v, s);
    }

    return v;
}

StrVec *Path_Ext(MemCh *m, StrVec *path){
    StrVec *v = StrVec_Make(m);

    Iter it;
    Iter_Init(&it, path->p);
    while((Iter_Prev(&it) & END) == 0){
        Str *s = (Str *)Iter_Get(&it);
        if(s->type.state & LAST){
            break;
        }
    }

    it.type.state &= ~END;
    while((Iter_Next(&it) & END) == 0){
        Str *s = (Str *)Iter_Get(&it);
        StrVec_Add(v, s);
    }

    return v;
}

StrVec *Path_Name(MemCh *m, StrVec *path){
    StrVec *v = StrVec_Make(m);

    Iter it;
    Iter_Init(&it, path->p);
    while((Iter_Next(&it) & END) == 0){
        Str *s = (Str *)Iter_Get(&it);
        if(s->type.state & MORE){
            v->total = 0;
            Span_Setup(v->p);
        }else if(s->type.state & LAST){
            break;
        }else{
            StrVec_Add(v, s);
        }
    }

    return v;
}

boolean Path_ExtEquals(StrVec *path, Str *ext){
    return Equals((Abstract *)Span_Get(path->p, path->p->max_idx), (Abstract *)ext);
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
        if(s->length == 0){
            continue;
        }
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
