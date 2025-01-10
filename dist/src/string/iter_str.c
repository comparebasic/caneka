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
    return (Abstract *)it->cursor->ptr;
}

status IterStr_Init(IterStr *it, String *s, size_t sz, byte *item){
    memset(it, 0, sizeof(IterStr));
    it->type.of = TYPE_ITER_STRING;
    it->sz = sz;
    it->idx = -1;
    it->item = item;
    it->count = String_Length(s) / sz;

    return SCursor_Init(&it->cursor, s);
}

IterStr *IterStr_Make(MemCtx *m){
    IterStr *it = (IterStr *)MemCtx_Alloc(m, sizeof(IterStr));
    it->type.of = TYPE_ITER_STRING;
    return it;
}
