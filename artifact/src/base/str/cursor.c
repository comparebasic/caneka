#include <external.h>
#include <caneka.h>

static status Cursor_SetStr(Cursor *curs){
    DebugStack_Push(curs, curs->type.of);
    Iter_GetByIdx(&curs->it, curs->it.idx);
    if(curs->it.type.state & SUCCESS){
        Str *s = (Str *)curs->it.value;
        if(s != NULL){
            curs->ptr = s->bytes;
            curs->end = s->bytes+(s->length-1);
            curs->type.state |= PROCESSING;
            DebugStack_Pop();
            return SUCCESS;
        }else{
            curs->ptr = NULL;
            curs->end = NULL;
        }
    }
    DebugStack_Pop();
    return NOOP;
}

status Cursor_Decr(Cursor *curs, i32 length){
    DebugStack_Push(curs, curs->type.of);
    if((curs->type.state & PROCESSING) == 0){
        Fatal(FUNCNAME, FILENAME, LINENUMBER,
            "Unable to decr cursor that is not in PROCESSING", NULL);
        DebugStack_Pop();
        return ERROR;
    }
    curs->type.state &= ~NOOP;
    Str *s = curs->it.value;
    i32 remaining = 0;
    if(s != NULL){
        remaining = (curs->ptr - s->bytes);
    }
    while(length > 0){
        if(length > remaining){
            length -= remaining;
            if(curs->it.idx == 0 && length > 0){
                Abstract *args[] = {
                    (Abstract *)I32_Wrapped(ErrStream->m,length),
                    NULL,
                };
                Fatal(FUNCNAME, FILENAME, LINENUMBER, "Unable to proceed back, length $", args);
                break;
            }else{
                curs->it.idx--;
                Cursor_SetStr(curs);
                byte *start = curs->ptr;
                curs->ptr = curs->end;
                remaining = (curs->ptr - start);
                length -= 1;
            }
        }else{
            curs->ptr -= length;
            DebugStack_Pop();
            return curs->type.state;
        }
    }
    DebugStack_Pop();
    return curs->type.state;
}

StrVec *Cursor_Get(MemCh *m, Cursor *_curs, i32 length, i32 offset){
    StrVec *v = StrVec_Make(m);
    Cursor *curs = Cursor_Copy(m, _curs);
    if(length < 0){
        length = abs(length);
        Cursor_Decr(curs, length);
    }
    if(offset < 0){
        offset = abs(offset);
        Cursor_Decr(curs, offset);
    }else if(offset > 0){
        Cursor_Incr(curs, offset);
    }
    while(length > 0){
        i64 taken = min(curs->end - curs->ptr, length);
        StrVec_AddBytes(m, v, curs->ptr, taken);
        length -= taken;
        curs->ptr += (taken-1);
        if((Cursor_NextByte(curs) & END) != 0){
            break;
        }

    }
    return v;
}

status Cursor_SetNextStr(Cursor *curs, Str *s, i32 max){
    status r = READY;
    if(max < 0){
        Error(ErrStream->m, (Abstract *)curs, FUNCNAME, FILENAME, LINENUMBER, 
            "Max is less than zero when requesting the next cursor str");
        return ERROR;
    }
    if(curs->ptr == NULL){
        r |= Cursor_SetStr(curs);
    }
    if(r & SUCCESS)
        s->bytes = curs->ptr;
        s->length = min(curs->end-curs->start+1, max);
        s->alloc = s->length;
    }else{
        s->bytes = NULL;
        s->length = 0
        s->alloc = 0;
        r |= NOOP;
    }
    return r;
}

status Cursor_Incr(Cursor *curs, i32 length){
    DebugStack_Push(curs, curs->type.of);
    if(curs->ptr == NULL){
        Cursor_SetStr(curs);
    }
    curs->type.state &= ~NOOP;
    Str *s = curs->it.value;
    i32 remaining = (curs->end - curs->ptr);
    while(length > 0){
        if(length > remaining){
            length -= remaining;
            curs->it.idx++;
            if(Cursor_SetStr(curs) & NOOP){
                curs->type.state |= NOOP;
                break;
            }else{
                s = curs->it.value;
                remaining = (curs->end - curs->ptr);
                length -= 1;
            }
        }else{
            curs->ptr += length;
            DebugStack_Pop();
            return curs->type.state;
        }
    }
    DebugStack_Pop();
    return curs->type.state;
}

status Cursor_NextByte(Cursor *curs){
    curs->type.state &= ~CURSOR_STR_BOUNDRY;
    if((curs->type.state & PROCESSING) == 0){
        curs->it.idx = 0;
        Cursor_SetStr(curs);
    }else if(curs->ptr >= curs->end){
        if(curs->it.type.state & LAST){
            curs->type.state |= END;
        }else{
            curs->it.idx++;
            Iter_Query(&curs->it);
            curs->type.state |= CURSOR_STR_BOUNDRY;
            Cursor_SetStr(curs);
        }
    }else if(curs->ptr < curs->end){
        curs->ptr++;
        curs->type.state |= SUCCESS;
    }else{
        curs->type.state |= NOOP;
    }
    return curs->type.state;
}

Cursor *Cursor_Copy(MemCh *m, Cursor *_curs){
    DebugStack_Push(_curs, _curs->type.of);
    Cursor *curs = MemCh_Alloc(m, sizeof(Cursor));
    memcpy(curs, _curs, sizeof(Cursor));
    DebugStack_Pop();
    return curs;
}

status Cursor_Add(Cursor *curs, Str *s){
    status r =  StrVec_Add(curs->v, s);
    if(curs->type.state & END){
        curs->type.state &= ~END;
        curs->it.type.state &= ~LAST;
    }else{
        curs->type.state &= ~END;
    }
    return r;
}

status Cursor_Setup(Cursor *curs, StrVec *v){
    curs->type.of = TYPE_CURSOR;
    curs->type.state &= DEBUG;
    curs->slot = 0;
    curs->offset = 0;
    curs->v = v;
    curs->ptr = NULL;
    curs->end = NULL;
    Iter_Init(&curs->it, v->p);
    return curs->it.type.state;
}

Cursor *Cursor_Make(MemCh *m, StrVec *v){
    Cursor *curs = (Cursor *)MemCh_Alloc(m, sizeof(Cursor));
    curs->type.of = TYPE_CURSOR;
    Cursor_Setup(curs, v);
    return curs;
}
