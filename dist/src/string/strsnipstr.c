#include <external.h>
#include <caneka.h>

String *StrSnipStr_ToString(MemCtx *m, String *sns, String *s){
    Stack(bytes("StrSnipStr_ToString"), NULL);
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
        printf("\x1b[%dmCopy %d from %d\x1b[0m\n", COLOR_CYAN, sn->length, sn->start);

        i64 start = sn->start;
        i64 remaining = sn->length;

        while(s != NULL && start > s->length){
            s = String_Next(s);
            start -= s->length;
        }

        while(s != NULL && remaining > 0){
            i64 length = min(s->length-start, remaining);
            ret->type.state |= DEBUG;
            String_AddBytes(m, ret, s->bytes+((int)start), length);
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
