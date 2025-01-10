#include <external.h>
#include <caneka.h>

status _Cursor_Init(MemCtx *m, Cursor *cur, String *s){
    cur->type.of = TYPE_CURSOR;
    cur->spaces = Span_MakeInline(m, TYPE_STRSNIP, (int)sizeof(StrSnip));
    cur->s = s;
    return SUCCESS;
}

status Cursor_Init(MemCtx *m, Cursor *cur, String *s){
    memset(cur, 0, sizeof(Cursor));
    return _Cursor_Init(m, cur, s);
}

status Cursor_ReInit(MemCtx *m, Cursor *cur, String *s){
    cur->s = s;
    memset(&cur->position, sizeof(StrSnipAbs));
    cur->position.type.of = TYPE_STRSNIP;
    cur->position.type.state |= STRSNIP_ABSOLUTE;
    Span_ReInit(cur->spaces);
    return _Cursor_Init(m, cur, s);
}

String *Cursor_ToString(MemCtx *m, Cursor *cur){
    String *ret = String_Init(m, STRING_EXTEND);

    String *s = cur->seg;
    i64 pos = cur->position.offset;

    Iter it;
    Iter_Init(&it, cur->spaces);
    while((Iter_Next(&it) & END) != 0){
        StrSnip *sn = (StrSnip *)Iter_Get(&it);
        if((sn->type.state & STRSNIP_COPIED) != 0){
            continue;
        }

        int start = sn->start;
        int end = sn->end;
        while(s != NULL && pos + s->length < start){
            s = String_Next(s);
            pos += s->length;
        }

        start -= pos;
        int length = min(s->length-start, end-pos);
        while(s != NULL && length > 0){
            int length = min(s->length-start, end-pos);
            String_AddBytes(m, ret, s->bytes+start, length);
            pos += length;
            s = String_Next(s); 
            start = 0;
        }

        sn->type.state |= STRSNIP_COPIED;
    }

    return ret;
}

Cursor *Cursor_Make(MemCtx *m, String *s){
    Cursor *cur = (Cursor *)MemCtx_Alloc(m, sizeof(Cursor));
    _Cursor_Init(m, cur, s);
    return cur;
};

status Cursor_AddMatch(Cursor *cur, int length){
    StrSnip sn;
    StrSnip *snip = Span_Get(cur->spaces, cur->spaces->metrics.set)
    if(snip != NULL && (snip->type.state & STRSNIP_GAP) == 0){
        snip->length += length;
    }else{
        StrSnip_Init(&sn, cur->position, length);
        Span_Add(cur->spaces, &sn);
    }
    return SUCCESS
}

status Cursor_AddGap(Cursor *cur, int length){
    StrSnip sn;
    StrSnip *snip = Span_Get(cur->spaces, cur->spaces->metrics.set)
    if(snip != NULL && (snip->type.state & STRSNIP_GAP) != 0){
        snip->length += length;
    }else{
        StrSnip_Init(&sn, cur->position, length);
        sn->type.state |= STRSNIP_GAP;
        Span_Add(cur->spaces, &sn);
    }
    return SUCCESS
}

status Cursor_Incr(Cursor *cur, int length){
    cur->position += length;
    return SUCCESS;
}
