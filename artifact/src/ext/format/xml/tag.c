#include <external.h>
#include <caneka.h>

i64 Tag_Out(Buff *bf, Abstract *name, word flags){
    i64 total = 0;
    if(flags & TAG_CLOSE){
        total += Buff_Bytes(bf, (byte *)"</", 2);
        total += ToS(bf, name, 0, ZERO);
        total += Buff_Bytes(bf, (byte *)">", 1);
    }else if(flags & TAG_SELFCLOSE){
        total += Buff_Bytes(bf, (byte *)"<", 1);
        total += ToS(bf, name, 0, ZERO);
        total += Buff_Bytes(bf, (byte *)"/>", 2);
    }else{
        total += Buff_Bytes(bf, (byte *)"<", 1);
        total += ToS(bf, name, 0, ZERO);
        total += Buff_Bytes(bf, (byte *)">", 1);
    }
    return total;
}
