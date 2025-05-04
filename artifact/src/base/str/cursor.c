#include <external.h>
#include <caneka.h>

static status Cursor_SetStr(Cursor *curs){
    if(Iter_Query(&curs->it) & SUCCESS){
        Str *s = (Str *)curs->it.value;
        if(s != NULL){
            curs->ptr = s->bytes;
            curs->end = s->bytes+(s->length-1);
            return SUCCESS;
        }else{
            curs->ptr = NULL;
            curs->end = NULL;
        }
    }
    return NOOP;
}

status Cursor_Decr(Cursor *curs, i32 length){
    DebugStack_Push(curs, curs->type.of);
    curs->type.state &= ~NOOP;
    Str *s = curs->it.value;
    i32 remaining = (curs->ptr - s->bytes);
    while(length > 0){
        if(length > remaining){
            length -= remaining;
            if(curs->it.idx == 0){
                curs->type.state |= NOOP;
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
    if(curs->ptr == NULL){
        return Cursor_SetStr(curs);
    }else if(curs->ptr == curs->end){
        if(curs->it.type.state & FLAG_ITER_LAST){
            curs->type.state |= END;
        }else{
            curs->it.idx++;
            Iter_Query(&curs->it);
            return Cursor_SetStr(curs) | CURSOR_STR_BOUNDRY;
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
    return StrVec_Add(curs->v, s);
}

status Cursor_Setup(Cursor *curs, StrVec *v){
    curs->type.of = TYPE_CURSOR;
    curs->type.state = READY;
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
