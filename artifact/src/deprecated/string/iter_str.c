#include <external.h>
#include <caneka.h>

static status IterStr_setShelf(IterStr *it){
    int taken = (it->cursor.seg->length - it->cursor.local);
    int remaining = it->sz - taken;
    memcpy(it->shelf, it->cursor.seg->bytes+it->cursor.local, taken);
    String *s = String_Next(it->cursor.seg);
    if(s != NULL){
        memcpy(it->shelf+taken, s->bytes, remaining);
    }else{
        memset(it->shelf+taken, 0, remaining);
        if((it->type.state & FLAG_ITERSTR_PAD) == 0){
            it->type.state |= (END|ERROR);
            return it->type.state;
        }
    }

    return it->type.state;
}

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
            if(it->cursor.type.state & FLAG_CURSOR_SEGMENTED){
                IterStr_setShelf(it);
            }
        }
    }

    return it->type.state;
}

void *IterStr_Get(IterStr *it){
    if(it->cursor.type.state & FLAG_CURSOR_SEGMENTED){
        return it->shelf;
    }
    return (void *)it->cursor.ptr;
}

status IterStr_Init(IterStr *it, String *s, size_t sz, void *shelf){
    memset(it, 0, sizeof(IterStr));
    it->type.of = TYPE_ITER_STRING;
    it->sz = sz;
    it->idx = -1;
    i64 total = String_Length(s);
    it->count = total / sz;
    if((total % sz) != 0){
        it->count++;
    }
    it->shelf = shelf;
    if(shelf == NULL && sz != 1 && (s->type.state & FLAG_STRING_CONTIGUOUS) == 0){
        Fatal("IterStr only works with contigously flagged strings unless a shelf is provided\n", TYPE_ITER_STRING);
        return ERROR;
    }
    Cursor_Init(&it->cursor, s);
    return SUCCESS;
}

IterStr *IterStr_Make(MemCtx *m, String *s, size_t sz, void *shelf){
    IterStr *it = (IterStr *)MemCtx_Alloc(m, sizeof(IterStr));
    IterStr_Init(it, s, sz, shelf);
    return it;
}
