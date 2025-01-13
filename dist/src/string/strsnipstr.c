#include <external.h>
#include <caneka.h>

String *StrSnipStr_ToString(MemCtx *m, String *sns, String *s){
    String *ret = String_Init(m, STRING_EXTEND);
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

        i64 start = sn->start;
        i64 remaining = sn->length;

        while(s != NULL && start > s->length){
            s = String_Next(s);
            start -= s->length;
        }

        while(s != NULL && remaining > 0){
            i64 length = min(s->length-start, remaining);
            String_AddBytes(m, ret, s->bytes+start, length);
            remaining -= length;
            start -= length;
            if(remaining > 0){
                s = String_Next(s);
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
