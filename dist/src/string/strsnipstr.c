#include <external.h>
#include <caneka.h>

String *StrSnipStr_ToString(MemCtx *m, String *sns, String *s){
    Stack(bytes("StrSnipStr_ToString"), NULL);
    String *ret = String_Init(m, STRING_EXTEND);

    i64 pos = 0;
    IterStr it;
    IterStr_Init(&it, sns, sizeof(StrSnip));
    while((IterStr_Next(&it) & END) == 0){
        if(s == NULL){
            break;
        }
        StrSnip *sn = (StrSnip *)IterStr_Get(&it);
        if(sn->length <= 0 || (sn->type.state & SUCCESS) == 0){
            continue;
        }

        i64 start = sn->start - pos;
        i64 remaining = sn->length;

        while(s != NULL && start > s->length){
            s = String_Next(s);
            start -= s->length;
        }

        while(s != NULL && remaining > 0){
            i64 length = min(s->length-start, remaining);
            String_AddBytes(m, ret, s->bytes+((int)start), length);
            remaining -= length;
            start -= length;
            if(remaining > 0){
                pos += s->length;
                s = String_Next(s);
                start = 0;
            }
        }
    }

    if(DEBUG_STRSNIP){
        Debug_Print((void *)sns,
            TYPE_STRSNIP_STRING, "StrSnipStr_ToString (sns): ", DEBUG_STRSNIP, TRUE);
        printf("\n");
        DPrint((Abstract *)ret, DEBUG_STRSNIP, "StrSnipStr_ToString: ");
        printf("\n");
    }

    Return ret;
}

int StrSnipStr_Total(String *sns, word flags){
    Stack(bytes("StrSnipStr_Total"), NULL);
    IterStr it;
    int total = 0;
    IterStr_Init(&it, sns, sizeof(StrSnip));
    while((IterStr_Next(&it) & END) == 0){
        StrSnip *sn = (StrSnip *)IterStr_Get(&it);
        if((sn->type.state & flags) == (flags) || (sn->type.state & flags) != 0){
            total += sn->length;
        }
    }

    Return total;
}
