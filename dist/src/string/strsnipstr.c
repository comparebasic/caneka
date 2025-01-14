#include <external.h>
#include <caneka.h>

String *StrSnipStr_ToString(MemCtx *m, String *sns, String *s){
    Stack(bytes("StrSnipStr_ToString"), NULL);
    String *ret = String_Init(m, STRING_EXTEND);
    IterStr it;
    IterStr_Init(&it, sns, sizeof(StrSnip));

    while((IterStr_Next(&it) & END) == 0){
        printf("%d", it.idx);
        fflush(stdout);
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
            printf("b");
            fflush(stdout);
            s = String_Next(s);
            start -= s->length;
        }

        while(s != NULL && remaining > 0){
            printf("c%ld/%ld", start, remaining);
            fflush(stdout);
            i64 length = min(s->length-start, remaining);
            printf("\n1 - %d: %s\n", (int)start, s->bytes+(int)start);
            String_AddBytes(m, ret, s->bytes+((int)start), length);
            printf("\n2\n");
            remaining -= length;
            start -= length;
            printf("%ld\n", remaining);
            if(remaining > 0){
                s = String_Next(s);
            }
            printf("c - end");
            fflush(stdout);
        }
        printf("?\n");
    }


    if(DEBUG_STRSNIP){
        Debug_Print((void *)sns,
            TYPE_STRSNIP_STRING, "StrSnipStr_ToString (sns): ", DEBUG_STRSNIP, TRUE);
        printf("\n");
        DPrint((Abstract *)ret, DEBUG_STRSNIP, "StrSnipStr_ToString: ");
        printf("\n");
    }

    printf("end");

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
