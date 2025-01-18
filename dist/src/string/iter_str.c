#include <external.h>
#include <caneka.h>

status IterStr_Next(IterStr *it){
    if(it->cursor.s == NULL){
        it->type.state |= END;
        return it->type.state;
    }
    if((it->type.state & END) != 0){
        it->idx = -1;
        it->type.state &= ~(END|FLAG_ITER_LAST);
    }
    if((it->idx+1) >= it->count){
        it->type.state |= END;
    }else{
        it->idx++;
        it->type.state |= SUCCESS;
        if(it->idx == (it->count-1)){
            it->type.state |= FLAG_ITER_LAST;
        }
        if(it->idx > 0){
            Cursor_Incr(&it->cursor, it->sz);
        }
    }

    return it->type.state;
}

Abstract *IterStr_Get(IterStr *it){
    return (Abstract *)it->cursor.ptr;
}

status IterStr_Init(IterStr *it, String *s, size_t sz){
    memset(it, 0, sizeof(IterStr));
    it->type.of = TYPE_ITER_STRING;
    it->sz = sz;
    it->idx = -1;
    it->count = String_Length(s) / sz;
    if(sz != 1 && (s->type.state & FLAG_STRING_CONTIGUOUS) == 0){
        Fatal("IterStr only works with contigously flagged strings\n", TYPE_ITER_STRING);
        return ERROR;
    }
    Cursor_Init(&it->cursor, s);
    return SUCCESS;
}

IterStr *IterStr_Make(MemCtx *m, String *s, size_t sz){
    IterStr *it = (IterStr *)MemCtx_Alloc(m, sizeof(IterStr));
    IterStr_Init(it, s, sz);
    return it;
}
