#include <external.h>
#include <caneka.h>

Str *Str_FromTyped(MemCtx *m, void *v, cls type){
    if(v == NULL){
        return Str_Ref(m, (byte *)"NULL", 4, 5);
    }else if(type == TYPE_STR){
        return (Str *)v;
    }else{
        Str *s = Str_Ref(m, (byte *)"UNKNOWN", 4, 5);
        s->type.state |= ERROR;
        return s;
    }
}

Str *Str_FromAbs(MemCtx *m, Abstract *a){
    if(a == NULL){
        return Str_Ref(m, (byte *)"NULL", 4, 5);
    }else{
        return Str_FromTyped(m, (void *)a, a->type.of);
    }
}
