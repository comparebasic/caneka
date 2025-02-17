#include <external.h>
#include <caneka.h>

status _Cursor_Init(Cursor *cur, String *s){
    cur->type.of = TYPE_CURSOR;
    cur->seg = cur->s = s;
    cur->ptr = s->bytes;
    return SUCCESS;
}

i64 Cursor_GetPad(Cursor *cur, size_t sz){
    i64 pad = 0;
    if(String_Next(cur->seg) == NULL){
        i64 taken = (cur->seg->length - cur->local);
        pad = sz - taken;
    }
    return pad;
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

status Cursor_Flush(MemCtx *m, Cursor *cur, OutFunc func, Abstract *source){
    if(cur->seg == NULL){
        return NOOP;
    }
    i64 local = cur->local;
    status r = READY;
    if(local > 0){
       String *_s = String_Init(m, STRING_EXTEND); 
       String_AddBytes(m, _s, cur->seg->bytes+local, cur->seg->length-local);
       _s = String_Next(cur->seg);
       r = func(m, _s, source);
    }else{
       r = func(m, cur->seg, source);
    }

    if(r & SUCCESS){
        cur->offset = cur->seg->length-cur->local;
        cur->local = 0;
        cur->seg = String_Next(cur->seg);
        while(cur->seg != NULL){
            cur->offset += cur->seg->length;
        }
        return r;
    }else{
        return ERROR;
    }
}

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
           return cur->type.state;
        }else{
           offset += max;
           local -= max;
        }
    }

    if((cur->type.state & END) == 0){
        if((segSz - local) < length){
            cur->type.state |= FLAG_CURSOR_SEGMENTED;
        }else{
            cur->type.state &= ~FLAG_CURSOR_SEGMENTED;
        }
        cur->offset = offset;
        cur->local = local;
        cur->seg = seg;
        cur->ptr = cur->seg->bytes+cur->local;
        return SUCCESS;
    }else{
        return ERROR;
    }
}
