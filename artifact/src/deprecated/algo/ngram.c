#include <external.h>
#include <caneka.h>

Ngram *Ngram_AddTo(MemCtx *m, Ngram *g, String *s, i64 offset){
    int idx = 0;

    Ngram *next = (Ngram*)(((void *)g)+sizeof(Ngram));
    byte b = s->bytes[0];
    if((g->flags & NGRAM_NEW) != 0){
        if((g->flags & NGRAM_BRANCHES) != 0){
            g->low = b;
            g->high = b;
        }
    }

    if(g->flags == NGRAM_CONTENT){
        NgramContent *ct = (NgramContent *)g;
        int length = ct->length == -1 ? NGRAM_SEG_LENGTH : ct->length;
        while(ct->content[idx] == s->bytes[idx] 
                && idx < length 
                && idx < = s->length){
            idx++;
        }
        b = s->bytes[idx];

        if(idx < length){
            Ngram_AddTo(MemCtx *m, next, String *_s, idx);
            g->flags |= NGRAM_TRUNC;
            g->length = idx;
        }
        Ngram_AddTo(MemCtx *m, next, String *_s);
        /* TODO: add further content that may have other branches as well */
    }

    while(g->flags & NGRAM_BRANCHES){
        if(b < g->low){
           g->low = b; 
        }
        if(b > g->high){
           g->high = b; 
        }
        g++;
    }

    if((g->flags & NGRAM_SPACE) != 0){
        i64 pos = 0;
        while(pos < s->length){
            MemSlab *sl = MemCtx_GetSlab(m, (void *)g);
            if(MemSlab_Available(sl) < (sizeof(Ngram)) || g < sl->addr-sizeof(Ngram)){
                Ngram *jump = (NgramJump *)g;
                jump->flags = NGRAM_JUMP;
                jump->next = Ngram_AddTo(m, Ngram_Make(m, (next->flags & ~NGRAM_SPACE)), _s);
            }else{
                next = Ngram_Make(m, NGRAM_SPACE);
                g->flags = NGRAM_BRANCHES;
                g->low = g->high = b;
                g->ct = Ngram_Make(m, (next->flags & ~NGRAM_CONTENT));
                i64 len = min(NGRAM_SEG_LENGTH, s->length-offset);
                memcpy(g->ct->content, len); 
                offset += len; 
                if(offset < s->length){
                    Ngram_AddTo(m, next, _s, offset);
                }
            }
        }
    }
}

Ngram *Ngram_Make(MemCtx *m, flags8 flags){
    Ngram *g = MemCtx_Alloc(m, sizeof(Ngram));
    g->flags = (NGRAM_NEW|flags);
    return g;
}

NgramStart *Ngram_FromSpan(MemCtx *m, Ngram *g, Span *p){
    DebugStack_Push("Ngram_FromSpan", TYPE_CSTR);
    Iter it;
    Iter_Init(&it);
    while((Iter_Next(&it) & END) == 0){
        String *s = (String *)asIfc(Iter_Get(&it), TYPE_STRING);
        Ngram_Add(m, g, s);
    }
    DebugStack_Pop();
    return NULL;
}
