#include <external.h>
#include <caneka.h>

i64 MemCh_MemCount(MemCh *m, i16 level){
    i64 total = 0;
    while((Iter_Next(&m->it) & END) == 0){
        MemPage *sl = (MemPage *)m->it.value;
        if(sl != NULL && (level == 0 || sl->level == level)){
            total += MEM_SLAB_SIZE;
        }
    }

    return total;
}

MemPage *MemCh_AddPage(MemCh *m, i16 level){
    MemPage *pg = MemPage_Make(m, level);
    Iter_Add(&m->it, (void *)pg);
    return pg;
}

void *MemCh_Alloc(MemCh *m, size_t sz){
    return MemCh_AllocOf(m, sz, 0);
}

void *MemCh_AllocOf(MemCh *m, size_t sz, cls typeOf){
    Abstract *args[3];
    if(m == NULL){
        Fatal(FUNCNAME, FILENAME, LINENUMBER, "MemCh is NULL", NULL);
        return NULL;
    }
    if(m->type.of != TYPE_MEMCTX){
        Fatal(FUNCNAME, FILENAME, LINENUMBER, "MemCh is missing type.of", NULL);
        return NULL;
    }
    if(sz > MEM_SLAB_SIZE){
        Fatal(FUNCNAME, FILENAME, LINENUMBER, "Trying to allocation too much memory at once", NULL);
    }

    if(m->it.p == NULL){
        Fatal(FUNCNAME, FILENAME, LINENUMBER, "Whaaaaat m->it.p is NULL?", NULL);
    }

    if(!Guard(&m->guard, MEM_GUARD_MAX, "MemCh_Alloc", FILENAME, LINENUMBER)){
        byte _b[128];
        memset(_b, 0, 128);
        Str s;
        s.type.of = TYPE_STR;
        s.bytes = _b;
        s.alloc = 128;
        Str_AddCstr(&s, "Guard Error allocating ");
        Str_AddCstr(&s, Type_ToChars(typeOf));
        Fatal(FUNCNAME, FILENAME, LINENUMBER, (char *)s.bytes, NULL);
        return NULL;
    }

    i16 level = 0;
    if((m->type.state & MEMCH_BASE) == 0){
        level = m->level;
    }

    word _sz = (word)sz;

    MemPage *sl = NULL;
    MemCh_SetToBase(m);
    while((Iter_Next(&m->it) & END) == 0){
        MemPage *_sl = (MemPage *)m->it.value;
        if(_sl != NULL && (level == 0 || _sl->level == level) && _sl->remaining >= _sz){
            sl = _sl;
            break;
        }
    }

    if(sl == NULL){
        if(m->type.state & DEBUG){
            args[0] = (Abstract *)I32_Wrapped(ErrStream->m, m->it.p->nvalues+1);
            args[1] = (Abstract *)I8_Wrapped(ErrStream->m, m->it.p->dims);
            args[2] = NULL;
            Out("^p.MemCh adding pages $/^D.$^d.dims currently^0\n", args);
            if(m->it.p->nvalues == 255){
                m->it.p->type.state |= DEBUG;
            }
        }

        sl = MemCh_AddPage(m, level);
        if(m->type.state & DEBUG){
            args[0] = (Abstract *)I32_Wrapped(ErrStream->m, MemBook_GetPageIdx(sl));
            args[1] = (Abstract *)I8_Wrapped(ErrStream->m, m->it.p->dims);
            args[2] = (Abstract *)NULL;
            Out("^p.    MemCh added page $/^D.$^d.dims currently^0\n", args);
        }
    }

    m->it.type.state = (m->it.type.state & NORMAL_FLAGS) | SPAN_OP_GET;
    Iter_Reset(&m->it);

    Guard_Reset(&m->guard);
    MemCh_SetFromBase(m);
    return MemPage_Alloc(sl, _sz);
}

i64 MemCh_Used(MemCh *m, i16 level){
    i64 total = 0;
    while((Iter_Next(&m->it) & END) == 0){
        MemPage *sl = (MemPage *)m->it.value;
        if(sl != NULL && (level == 0 || sl->level == level)){
            total += MEM_SLAB_SIZE - sl->remaining;
        }
    }

    return total;
}

void *MemCh_Realloc(MemCh *m, size_t s, void *orig, size_t origsize){
    if(s > origsize){
        Fatal(FUNCNAME, FILENAME, LINENUMBER, "Asking to copy more than newly allocated", NULL);
        return NULL;
    }
    void *p = MemCh_Alloc(m, s);
    memcpy(p, orig, origsize);
    return p; 
}

i64 MemCh_Total(MemCh *m, i16 level){
    return MemCh_MemCount(m, level);
}

status MemCh_WipeTemp(MemCh *m, i16 level){
    status r = READY;

    while((Iter_Next(&m->it) & END) == 0){
        MemPage *pg = (MemPage *)m->it.value;
        if(pg != NULL && (level == 0 || pg->level >= level) && pg->remaining < MEM_SLAB_SIZE){
            size_t sz = MemPage_Taken(pg); 
            void *ptr = pg;
            ptr += sizeof(MemPage);
            ptr += pg->remaining;
            memset(ptr, 0, sz);
            pg->remaining = MEM_SLAB_SIZE;
            r = SUCCESS;
        }
    }

    if(r == READY){
        r |= NOOP;
    }

    Iter_Reset(&m->it);

    return r;
}

status MemCh_FreeTemp(MemCh *m, i16 level){
    status r = READY;
    level = max(level, 0);
    if(m->type.state & DEBUG){
        Abstract *args[] = {
            (Abstract *)I32_Wrapped(ErrStream->m, m->it.p->nvalues),
            (Abstract *)I16_Wrapped(ErrStream->m, level),
            NULL
        };
        Out("^p.MemCh_FreeTemp ^D.$^d.nvalues ^D.$^d.level^0\n", args);
    }

    m->it.type.state |= FLAG_ITER_REVERSE;
    while((Iter_Next(&m->it) & END) == 0){
        if(m->type.state & DEBUG){
            Abstract *args[] = {
                (Abstract *)I32_Wrapped(ErrStream->m, m->it.idx),
                (Abstract *)I32_Wrapped(ErrStream->m, m->it.p->max_idx),
                (Abstract *)I16_Wrapped(ErrStream->m, level),
                NULL
            };
            Out("^p.    MemCh_FreeTemp \\@$of$/^D.$^d.level\n", args);
        }
        if(m->it.idx == 0){
            continue;
        }
        MemPage *pg = (MemPage *)m->it.value;
        if(pg != NULL && (level == 0 || pg->level >= level)){
            if(m->it.idx > 0){
                r |= Span_Remove(m->it.p, m->it.idx);
            }
            r |= MemBook_FreePage(m, pg);
        }
        if(m->type.state & DEBUG){
            Abstract *args[] = {
                (Abstract *)I32_Wrapped(ErrStream->m, m->it.idx),
            (Abstract *)I16_Wrapped(ErrStream->m, level),
                NULL
            };
            Out("^p.    done -MemCh_FreeTemp \\@$/^D.$^d.level\n", args);
        }
    }

    if(r == READY){
        r |= NOOP;
    }
    Iter_Reset(&m->it);

    return r;
}

status MemCh_Free(MemCh *m){
    status r = MemCh_FreeTemp(m, m->level);
    if(m->level == 0){
        return MemBook_FreePage(m, m->first);
    }
    return r;
}

void *MemCh_GetPage(MemCh *m, void *addr, i32 *idx){
    Iter_Reset(&m->it);
    while((Iter_Next(&m->it) & END) == 0){
        MemPage *pg = (MemPage *)m->it.value;
        if(pg != NULL){
            void *start = (void *)pg;
            void *end = (void *)pg;
            end += sizeof(MemPage)+MEM_SLAB_SIZE;
            if(addr >= start && addr <= end){
                *idx = m->it.idx;
                return pg;
            }
        }
    }

    Iter_Reset(&m->it);
    *idx = -1;
    return NULL;
}

status MemCh_Setup(MemCh *m, MemPage *pg){
    m->type.of = TYPE_MEMCTX;
    m->first = pg;
    Span *p = MemPage_Alloc(pg, sizeof(Span));
    Span_Setup(p);
    p->m = m;
    p->max_idx = -1;
    p->root = (slab *)Bytes_AllocOnPage(pg, sizeof(slab), TYPE_POINTER_ARRAY);
    Iter_Init(&m->it, p);
    status r = Iter_SetByIdx(&m->it, 0, (void *)pg);
    m->it.type.state = ((m->it.type.state & NORMAL_FLAGS) | SPAN_OP_GET);
    return r;
}

MemCh *MemCh_OnPage(){
    MemPage *sl = (MemPage *)MemPage_Make(NULL, 0);
    MemCh *m = (MemCh *)MemPage_Alloc(sl, sizeof(MemCh));
    MemCh_Setup(m, sl);
    return m;
}

MemCh *MemCh_Make(){
    MemCh *m = MemCh_OnPage();
    return m;
}
