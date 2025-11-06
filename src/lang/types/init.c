#include <external.h>
#include <caneka.h>

status Lang_Init(MemCh *m){
    status r = READY;
    r |= LangTypeStrings_Init(m);
    r |= Templ_ClsInit(m);
    return r;
}
