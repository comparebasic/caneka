#include <external.h>
#include <caneka.h>

i64 Sequence_Init(MemCh *m){
    status r = READY;
    r |= Sequence_ClassInit(m, ClassLookup); 
    return r;
}
