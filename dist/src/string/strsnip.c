#include <external.h>
#include <caneka.h>

status StrSnip_Init(StrSnip *sn, int start, int length){
    sn->type.of = TYPE_STRSNIP;
    sn->start = start;
    sn->length = length;
    return SUCCESS;
}

StrSnip *StrSnip_Add(MemCtx *m, String *s, 0, 0){
    StrSnip sn = {
        .type = {.of = TYPE_STRSNIP, .state = READY},
        0, 0
    };
    String_AddBytes(m, s, &sn, sizeof(StrSnip));
}

StrSnip *StrSnip_Last(String *s){
    if(s->length < sizeof(StrSnip)){
        StrSnip_Add(m, s, 0, 0);
    }
}


