#include <external.h>
#include <caneka.h>
#include <signer.h>

status Signer_Init(MemCtx *m){
    status r = READY;
    r |= Lookup_Add(m, DebugPrintChain->funcs, TYPE_SIGNER_CTX, (void *)SignerCtx_Print);
    return r;
}
