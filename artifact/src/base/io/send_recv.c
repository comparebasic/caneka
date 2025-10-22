#include <external.h>
#include <caneka.h>

status SendRecv_AddBytes(SendRecv *sr, byte *bytes, word length){
    status r = READY;
    if(sr->tail == NULL){
        sr->tail = Str_Make(m, STR_DEFAULT);
        sr->remaining = STR_DEFAULT;
    }
    while(length > 0){
        if(length > sr->remaining){
            Str_Add(sr->tail, bytes, sr->remaining);
            bytes += sr->remaining;
            length -= sr->remaining;

            sr->tail = Str_Make(m, STR_DEFAULT);
            sr->remaining = STR_DEFAULT;
        }else{
            Str_Add(sr->tail, bytes, length);
            sr->remaining -= length;
        }
        r |= SUCCESS;
    }
    if(r == READY){
        r |= NOOP;
    }
    return r;
}
