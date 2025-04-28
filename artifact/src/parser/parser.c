#include <external.h>
#include <caneka.h>

status Parser_Init(MemCh *m){
    return Parser_ToSInit(m, ToStreamLookup);
}
