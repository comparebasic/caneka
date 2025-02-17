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

    if(ctx->configPath != NULL){
        while((SignerCtx_DigestIdent(ctx) & (SUCCESS|ERROR)) == 0){
            printf("digesting ident file\n");
        }
    }

    DPrint((Abstract *)ctx, COLOR_PURPLE, "Ctx: ");

    MemCtx_Free(m);
    DebugStack_Pop();
    return 0;
}
