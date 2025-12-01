#include <external.h>
#include "base_module.h" 

boolean CanCombine(void *_a, void *_b){
    Abstract *a = (Abstract *)_a;
    Abstract *b = (Abstract *)_b;
    if(a->type.of == TYPE_STR && a->type.of == b->type.of){
        return TRUE;
    }else if(a->type.of == TYPE_STRVEC){
        if(a->type.of == b->type.of){
            return TRUE;
        }else if(b->type.of == TYPE_STR){
            return TRUE;
        }
    }else if(a->type.of == TYPE_SPAN){
        if(b->type.of != TYPE_SPAN){
            return TRUE;
        }
    }
    return FALSE;
}

boolean Combine(void *_a, void *_b){
    Abstract *a = (Abstract *)_a;
    Abstract *b = (Abstract *)_b;
    if(a->type.of == TYPE_STR && a->type.of == b->type.of){
        Str *s = (Str *)b;
        return Str_Add((Str *)a, s->bytes, s->length) == s->length;
    }else if(a->type.of == TYPE_STRVEC){
        if(a->type.of == b->type.of){
            return (StrVec_AddVec((StrVec *)a, (StrVec *)b) & SUCCESS) != 0;
        }else if(b->type.of == TYPE_STR){
            return (StrVec_Add((StrVec*)a, (Str*)b) & SUCCESS) != 0;
        }
    }else if(a->type.of == TYPE_SPAN){
        Span *p = (Span *)a;
        if(b->type.of != TYPE_SPAN){
            Iter it;
            Iter_Setup(&it, p, SPAN_OP_ADD, p->max_idx);
            it.value = b;
            return (Iter_Query(&it) & SUCCESS) != 0;
        }
    }
    return FALSE;
}
