#include <external.h>
#include <caneka.h>

status IterStr_Next(IterStr *it){
    Stack(bytes("IterStr_Next"), NULL);
    if(it->cursor.s == NULL){
        it->type.state |= END;
        Return it->type.state;
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

    Return it->type.state;
}

Abstract *IterStr_Get(IterStr *it){
    return (Abstract *)it->cursor.ptr;
}

status IterStr_Init(IterStr *it, String *s, size_t sz){
    it->type.of = TYPE_ITER_STRING;
    it->sz = sz;
    it->idx = -1;
    it->count = String_Length(s) / sz;
    Cursor_Init(&it->cursor, s);
    return SUCCESS;
}

IterStr *IterStr_Make(MemCtx *m, String *s, size_t sz){
    IterStr *it = (IterStr *)MemCtx_Alloc(m, sizeof(IterStr));
    IterStr_Init(it, s, sz);
    return it;
}
