#include <external.h>
#include <caneka.h>
#include <signer.h>

typedef status (*charSetterFunc)(char *arg, char *value);
typedef struct si_args  {
    char *arg;
    charSetterFunc func;
} SiArgs;

static char *filePath = NULL;
static char *configFilePath = NULL;

void Cleanup(Abstract *a){
    return;
}

status setFileArg(char *arg, char *value){
    filePath = value;
    return SUCCESS;
}

status setIdentityFileArg(char *arg, char *value){
    configFilePath = value;
    return SUCCESS;
}

SiArgs argFuncs[] =  {
    {"-i", setIdentityFileArg},
    {"-f", setFileArg},
    {NULL, NULL},
};

int main(int argc, char *argv[]){
    status r = READY;

    MemCtx *m = MemCtx_Make();
    Caneka_Init(m);
    DebugStack_Push("signer", TYPE_CSTR); 

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
                exit(1);
            }
        }else{
            func(prev, arg);
            func = NULL;
        }
    }

    printf("have %s and %s\n", filePath, configFilePath);

    MemCtx_Free(m);

    DebugStack_Pop();
    return r;
}
