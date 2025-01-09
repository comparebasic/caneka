#include <external.h>
#include <caneka.h>

status StrSnip_Init(StrSnip *sn, int start, int length){
    sn->type.of = TYPE_STRSNIP;
    sn->start = start;
    sn->length = length;
    return SUCCESS;
}
