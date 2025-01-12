#include <external.h>
#include <caneka.h>

status StrSnip_Init(StrSnip *sn, word flags, int start, int length){
    memset(sn, 0, sizeof(StrSnip));
    sn->type.of = TYPE_STRSNIP;
    sn->type.state = flags;
    sn->start = start;
    sn->length = length;
    return SUCCESS;
}

String *StrSnip_ToString(MemCtx *m, String *sns, String *s){
    String *ret = String_Init(m, STRING_EXTEND);
    i64 pos = 0;
    IterStr it;
    IterStr_Init(&it, sns, sizeof(StrSnip));
    while((IterStr_Next(&it) & END) != 0){
        StrSnip *sn = (StrSnip *)IterStr_Get(&it);

        int start = sn->start;
        int end = sn->start+sn->length;
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
    }

    return ret;
}

int StrSnipStr_Total(String *sns, word flags){
    IterStr it;
    int total = 0;
    IterStr_Init(&it, sns, sizeof(StrSnip));
    while((IterStr_Next(&it) & END) == 0){
        StrSnip *sn = (StrSnip *)IterStr_Get(&it);
        if((sn->type.state & flags) == (flags) || (sn->type.state & flags) != 0){
            total += sn->length;
        }
    }
    return total;
}

