#include <external.h>
#include <caneka.h>

i64 Tag_Out(Stream *sm, Abstract *name, word flags){
    i64 total = 0;
    if(flags & TAG_CLOSE){
        total += Stream_Bytes(sm, (byte *)"</", 2);
        total += ToS(sm, name, 0, ZERO);
        total += Stream_Bytes(sm, (byte *)">", 1);
    }else if(flags TAG_SELFCLOSE){
        total += Stream_Bytes(sm, (byte *)"<", 1);
        total += ToS(sm, name, 0, ZERO);
        total += Stream_Bytes(sm, (byte *)"/>", 2);
    }else{
        total += Stream_Bytes(sm, (byte *)"<", 1);
        total += ToS(sm, name, 0, ZERO);
        total += Stream_Bytes(sm, (byte *)">", 1);
    }
    return total;
}
