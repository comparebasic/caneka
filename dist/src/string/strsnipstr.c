#include <external.h>
#include <caneka.h>

String *StrSnipStr_ToString(MemCtx *m, String *sns, String *s){
    DebugStack_Push("StrSnipStr_ToString");
    String *ret = String_Init(m, STRING_EXTEND);

    i64 pos = 0;
    IterStr it;
    IterStr_Init(&it, sns, sizeof(StrSnip));
    while((IterStr_Next(&it) & END) == 0){
        if(s == NULL){
            break;
        }
        StrSnip *sn = (StrSnip *)IterStr_Get(&it);
        if(sn->length <= 0 || (sn->type.state & STRSNIP_CONTENT) == 0){
            continue;
        }

        i64 start = sn->start - pos;
        i64 remaining = sn->length;

        while(s != NULL && start > s->length){
            start -= s->length;
            s = String_Next(s);
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

    DebugStack_Pop();
    return ret;
}

int StrSnipStr_Total(String *sns, word flags){
    IterStr it;
    int total = 0;
    IterStr_Init(&it, sns, sizeof(StrSnip));
    while((IterStr_Next(&it) & END) == 0){
        StrSnip *sn = (StrSnip *)IterStr_Get(&it);
        if((sn->type.state & flags) == (flags) || flags == 0){
            total += sn->length;
        }
    }

    return total;
}
