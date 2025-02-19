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
    if(argc < 2){
        Signer_ShowArgs(ctx);
        exit(1);
    }else if(Signer_SetArgs(ctx, argc, argv) & ERROR){
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

    if(ctx->content == NULL){
        Fatal("No file contents to summarize", 0);
        exit(1);
    }

    SignerCtx_HeaderOut(ctx, ToStdOut);
    ToStdOut(ctx->m, ctx->content, NULL);
    SignerCtx_SetupSummary(ctx);
    if(SignerCtx_Sign(ctx) & ERROR){
        Fatal("Signing Error", 0);
        exit(1);
    }
    SignerCtx_SummaryOut(ctx, ToStdOut);

    MemCtx_Free(m);
    DebugStack_Pop();
    return 0;
}
