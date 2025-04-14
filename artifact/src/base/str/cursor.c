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
    Str *s = curs->it.value;
    while(--length > 0){
        if(curs->ptr == NULL){
            return ERROR;
        }else if(s != NULL && curs->ptr == s->bytes){
            if(curs->it.idx == 0){
                curs->type.state |= END;
            }else{
                curs->it.idx--;
                Iter_Query(&curs->it);
                return Cursor_SetStr(curs);
            }
        }else if(curs->ptr > s->bytes){
            curs->ptr--;
            curs->pos--;
            curs->type.state |= SUCCESS;
        }else{
            curs->type.state |= NOOP;
        }
    }
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
            return Cursor_SetStr(curs);
        }
    }else if(curs->ptr < curs->end){
        curs->ptr++;
        curs->pos++;
        curs->type.state |= SUCCESS;
    }else{
        curs->type.state |= NOOP;
    }
    return curs->type.state;
}

Cursor *Cursor_Copy(MemCh *m, Cursor *_curs){
    Cursor *curs = MemCh_Alloc(m, sizeof(Cursor));
    memcpy(_curs, curs, sizeof(Cursor));
    return curs;
}

status Cursor_Add(Cursor *curs, Str *s){
    StrVec_Add(curs->v, s);
    while((Iter_Next(&curs->it) & END) == 0 && curs->it.value != s){}
    return SUCCESS;
}

status Cursor_Setup(Cursor *curs, StrVec *v){
    curs->type.of = TYPE_STRVEC_CURS;
    curs->type.state = READY;
    curs->pos = 0;
    curs->slot = 0;
    curs->v = v;
    Iter_Init(&curs->it, v->p);
    return curs->it.type.state;
}

Cursor *Cursor_Make(MemCh *m, StrVec *v){
    Cursor *curs = (Cursor *)MemCh_Alloc(m, sizeof(Cursor));
    curs->type.of = TYPE_CURSOR;
    Cursor_Setup(curs, v);
    return curs;
}
