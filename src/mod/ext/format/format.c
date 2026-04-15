#include <external.h>
#include <caneka.h>

status Format_Init(MemCh *m){
    status r = READY;
    r |= FmtToHtml_Init(m);
    return r;
}
