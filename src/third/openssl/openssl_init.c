#include <external.h>
#include <caneka.h>

status Crypto_Init(MemCh *m){
    return OpenSsl_ExtFreeInit(m);;
}
