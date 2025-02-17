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
    File file;

    Signer_Init(m);

    SignerCtx *ctx = SignerCtx_Make(m);
    if(Signer_SetArgs(ctx, argc, argv) & ERROR){
        exit(1);
    }
    ctx->rbl = Kve_RblMake(m, NULL, (Abstract *)ctx->identTbl, Kve_Capture);

    if(ctx->filePath != NULL){
        File_Init(&file, ctx->filePath, NULL, NULL); 
        file.abs = File_GetCwdPath(ctx->m, file.path);
        if(File_Load(m, &file, NULL) & ERROR){
            Fatal("Unable to Load file\n", 0);
            exit(1);
        }
        ctx->content = file.data;
    }else{
        Fatal("FilePath not defined\n", 0);
        exit(1);
    }

    if(ctx->configPath != NULL){
        while((SignerCtx_DigestIdent(ctx) & (SUCCESS|ERROR)) == 0){
            printf("digesting ident file\n");
        }
    }

    /*
    DPrint((Abstract *)ctx, COLOR_PURPLE, "Ctx: ");
    DPrint((Abstract *)file.data, COLOR_CYAN, "File.data: ");
    */

    SignerCtx_HeaderOut(ctx, ToStdOut);
    ToStdOut(ctx->m, ctx->content, NULL);
    SignerCtx_SummaryOut(ctx, ToStdOut);

    MemCtx_Free(m);
    DebugStack_Pop();
    return 0;
}
