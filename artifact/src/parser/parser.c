#include <external.h>
#include <caneka.h>

status Parser_Init(MemCh *m){
    return ParserDebug_Init(m, DebugPrintChain);
}
