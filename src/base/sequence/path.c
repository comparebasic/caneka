#include <external.h>
#include <caneka.h>

static Span *dotPathSeps = NULL;

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

status Path_JoinBase(MemCh *m, StrVec *path){
    Iter it;
    Iter_Init(&it, path->p);
    status r = READY;
    i32 idx = 0;
    Str *prev = NULL;
    boolean last = FALSE;
    while((Iter_Next(&it) & END) == 0){
        Str *s = (Str *)Iter_Get(&it);
        if(s->type.state & LAST){
            last = TRUE;
            idx = it.idx;
        }
        if(last){
            if(prev->bytes+prev->length == s->bytes){
                prev->length += s->length;
                prev->alloc += s->length;
                idx = it.idx;
            }else{
                path->type.state |= ERROR;
                Error(m, FUNCNAME, FILENAME, LINENUMBER,
                    "Error tyring to join non-contiguous bytes", NULL);
                path->type.state |= ERROR;
                return path->type.state;
            }
        }else{
            prev = s;
        }
    }

    while(it.idx >= idx){
        Iter_PrevRemove(&it);
        r |= SUCCESS;
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

StrVec *Path_ReJoinExt(m, StrVec *v){
    StrVec *v = StrVec_Make(m);
    Iter it;
    Iter_Init(&it, path->p);
    word flags = ZERO;
    i32 fnameStart = path->p->max_idx;
    word length = 0;
    while((Iter_Prev(&it) & END) == 0){
        Str *s = (Str *)Iter_Get(&it);
        flags |= s->type.state & MORE;
        if(flags){
            break;
        }
        fnameStart--;
        length += s->length;
    }

    Str *s = Str_Make(m, length+1);
    Iter_Init(&it, path->p);
    while((Iter_Next(&it) & END) == 0){
        Str *_s = (Str *)Iter_Get(&it);
        if(it.idx < fnameStart){
            StrVec_Add(v, _s);
        }else{
            Str_Add(s, _s->bytes, s->length);
        }
        if(it.type.state & LAST){
            StrVec_Add(v, _s);
        }
    }

    return v;
}

StrVec *Path_WithoutExt(m, path){
    StrVec *v = StrVec_Make(m);
    Iter it;
    Iter_Init(&it, path->p);
    word flags = ZERO;
    i64 total = path->total;
    while((Iter_Prev(&it) & END) == 0){
        Str *s = (Str *)Iter_Get(&it);
        if((flags & MORE) == 0){
            total -= s->length;
            if(s->type.state & MORE){
                flags |= s->type.state & MORE;
            }
            continue;
        }else{
            Span_Set(v->p, s, it.idx);
        }
    }
    v->total = total;
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

status Path_DotAnnotate(MemCh *m, StrVec *v){
    return Path_Annotate(m, v, dotPathSeps);
}

status Path_Annotate(MemCh *m, StrVec *v, Span *sep){
    status r = READY;

    Span *p = Span_Clone(m, v->p);
    Span_Wipe(v->p);
    v->total = 0;

    Iter it;
    Iter sepIt;
    Iter_Init(&sepIt, sep);
    Iter_Init(&it, p);
    while((Iter_Next(&it) & END) == 0){
        Str *s = (Str *)asLegal(
            (Abstract *)Iter_Get(&it),
            TYPE_STR
        );
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
                    if(length < 0 || length > STR_MAX){
                        Error(p->m, FUNCNAME, FILENAME, LINENUMBER,
                            "Error cannot have a negative length of a string",
                        NULL);
                        return ERROR;
                    }
                    if(length > 0){
                        Str *sn = Str_Ref(m, start, length, length, ZERO);
                        StrVec_Add(v, sn);
                    }
                    Str *sb = Str_Ref(m, ptr, 1, 1, sg->objType.state);
                    StrVec_Add(v, sb);

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
        if(length < 0 || length > STR_MAX){
            Error(p->m, FUNCNAME, FILENAME, LINENUMBER,
                "Error cannot have a negative length of a string at end",
            NULL);
            return ERROR;
        }
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

status Path_Init(MemCh *m){
    if(dotPathSeps == NULL){
        MemCh_SetToBase(m);
        dotPathSeps = Span_Make(m);
        Span_Add(dotPathSeps, (Abstract *)B_Wrapped(m, (byte)'.', ZERO, MORE));
        MemCh_SetFromBase(m);
        return SUCCESS;
    }
    return NOOP;
}
