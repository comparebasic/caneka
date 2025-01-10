#include <external.h>
#include <caneka.h>

status StrSnip_Init(StrSnip *sn, int start, int length){
    sn->type.of = TYPE_STRSNIP;
    sn->start = start;
    sn->length = length;
    return SUCCESS;
}

StrSnip *StrSnip_Add(MemCtx *m, String *s, word flags, int start, int length){
    StrSnip sn = {
        .type = {.of = TYPE_STRSNIP, .state = flags},
        start, length 
    };
    String_AddBytes(m, s, &sn, sizeof(StrSnip));
}

status StrSnip_Incr(MemCtx *m, String *sns, int length){
    StrSnip *sn = StrSnip_Last(sns);
}

status StrSnip_Set(MemCtx *m, String *sns, int length){
    StrSnip *sn = StrSnip_Last(sns);
}

StrSnip *StrSnip_Last(MemCtx *m, String *s){
    if(s->length < sizeof(StrSnip)){
        StrSnip_Add(m, s, 0, 0);
    }
    String *next = s;
    while((next = String_Next(next)) != NULL){
        s = next;
    };
    return (StrSnip *)s->bytes+(s->length-sizeof(StrSnip));
}
