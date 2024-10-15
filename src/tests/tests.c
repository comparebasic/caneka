#include <external.h>
#include <caneka.h>
#include <tests.h>

status Tests_Init(MemCtx *m){
   status r = READY;
   r |= Make109Strings(m);
   return r;
}

