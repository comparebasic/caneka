#include <external.h>
#include <caneka.h>
#include <signer.h>

void Cleanup(Abstract *a){
    return;
}

int main(int argc, char *argv[]){
    status r = READY;

    MemCtx *m = MemCtx_Make();
    Caneka_Init(m);
    DebugStack_Push("signer", TYPE_CSTR); 

    Signer_Init(m);

    SignerCtx *ctx = SignerCtx_Make(m);
    if(Signer_SetArgs(ctx, argc, argv) & ERROR){
        exit(1);
    }
    ctx->rbl = Kve_RblMake(m, NULL, (Abstract *)ctx->identTbl, Kve_Capture);

    DPrint((Abstract *)ctx, COLOR_PURPLE, "Ctx: ");

    MemCtx_Free(m);
    DebugStack_Pop();
    return 0;
}
