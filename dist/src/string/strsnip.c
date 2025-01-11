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
