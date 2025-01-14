#include <external.h>
#include <caneka.h>

status _Cursor_Init(Cursor *cur, String *s){
    cur->type.of = TYPE_CURSOR;
    cur->seg = cur->s = s;
    cur->ptr = s->bytes;
    return SUCCESS;
}

status Cursor_Init(Cursor *cur, String *s){
    memset(cur, 0, sizeof(Cursor));
    cur->type.of = TYPE_CURSOR;
    return _Cursor_Init(cur, s);
}

Cursor *Cursor_Make(MemCtx *m, String *s){
    Cursor *cur = (Cursor *)MemCtx_Alloc(m, sizeof(Cursor));
    cur->type.of = TYPE_CURSOR;
    _Cursor_Init(cur, s);
    return cur;
};

status Cursor_Decr(Cursor *cur, int length){
   if(cur->local > length){
        cur->local -= length;
   }else{
        i64 pos = (cur->offset+cur->local)-length;
        cur->offset = cur->local = 0;
        Cursor_Incr(cur, pos);
   }
   return SUCCESS;
}

status Cursor_Incr(Cursor *cur, int length){
    Stack(bytes("Cursor_Incr"), NULL);

    i64 offset = cur->offset;
    i64 local = cur->local;;
    String *seg = cur->seg;

    i64 segSz = String_GetSegSize(cur->s);
    i64 max = segSz;
    if((cur->s->type.state & FLAG_STRING_CONTIGUOUS) != 0){
        max = segSz - (segSz % length);
    }

    local += length;
    if(local >= seg->length){
        seg = String_Next(seg);
        if(seg == NULL){
           cur->type.state |= END; 
           Return cur->type.state;
        }else{
           offset += max;
           local -= max;
        }
    }

    if((cur->type.state & END) == 0){
        cur->offset = offset;
        cur->local = local;
        cur->seg = seg;
        cur->ptr = cur->seg->bytes+cur->local;
        Return SUCCESS;
    }else{
        Return ERROR;
    }

}
