#include <external.h>
#include <caneka.h>

status _Cursor_Init(Cursor *cur, String *s){
    cur->type.of = TYPE_CURSOR;
    cur->seg = cur->s = s;
    return SUCCESS;
}

status Cursor_Init(Cursor *cur, String *s){
    memset(cur, 0, sizeof(Cursor));
    return _Cursor_Init(cur, s);
}

Cursor *Cursor_Make(MemCtx *m, String *s){
    Cursor *cur = (Cursor *)MemCtx_Alloc(m, sizeof(Cursor));
    _Cursor_Init(cur, s);
    return cur;
};

status Cursor_Incr(Cursor *cur, int length){
    cur->local += length;
    i64 segSz = String_GetSegSize(cur->s);
    i64 max = segSz;
    if((cur->s->type.state & FLAG_STRING_CONTIGUOUS) != 0){
        max = segSz - (segSz % length);
    }
    if(cur->local >= max){
        cur->offset += max;
        cur->local -= max;
        cur->seg = String_Next(cur->seg);
    }
    cur->ptr = cur->seg->bytes+cur->local;
    return SUCCESS;
}
