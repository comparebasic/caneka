#include <external.h>
#include <caneka.h>
#include <signer.h>

static char *filePath = NULL;
static char *configFilePath = NULL;

static status setFileArg(SignerCtx *ctx, char *arg, char *value){
    ctx->filePath = String_Make(ctx->m, bytes(value));
    return SUCCESS;
}

static status setIdentityFileArg(SignerCtx *ctx, char *arg, char *value){
    ctx->configPath = String_Make(ctx->m, bytes(value));
    return SUCCESS;
}

SiArgs argFuncs[] =  {
    {"-i", setIdentityFileArg},
    {"-f", setFileArg},
    {NULL, NULL},
};


status Signer_SetArgs(SignerCtx *ctx, int argc, char *argv[]){
    charSetterFunc func = NULL;
    char *prev = NULL;
    for(int i = 1; i < argc; i++){
        char *arg = argv[i];
        if(func == NULL){
            SiArgs *sa = argFuncs;
            while(sa->arg != NULL){
                if(strncmp(sa->arg, arg, strlen(arg)) == 0){
                    func = sa->func;
                    prev = sa->arg;
                    break;
                }
                sa++;
            }
            if(func == NULL){
                fprintf(stderr, "Error arg not found: %s\n", arg);
                ctx->type.state |= ERROR;
                return ctx->type.state;
            }
        }else{
            func(ctx, prev, arg);
            func = NULL;
        }
    }

    return ctx->type.state;
}
