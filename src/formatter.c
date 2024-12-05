#include <external.h>
#include <caneka.h>

int main(int argc, char **argv){
    if(argc == 1){
        printf("%s\n", argv[0]);
        exit(1);
    }

    MemCtx *m = MemCtx_Make();
    Caneka_Init(m);

    char *in = NULL;
    char *out = NULL;
    if(argc == 2){
        fprintf(stderr, "Reading from: '%s'\n", argv[1]);
        in = argv[1];
    }else if(argc == 3){
        fprintf(stderr, "Reading from: '%s' saving to '%s'\n", argv[1], argv[2]);
        in = argv[1];
        out = argv[2];
    }

    int ret = 0;

    FmtCtx *ctx = FmtHtml_Make(m);
    if(in != NULL){
        String *inpath = String_Make(m, bytes(in));
        File inf;
        File_Init(&inf, inpath, NULL, NULL);
        if((File_Load(m, &inf, NULL) & SUCCESS) != 0){
            Roebling_Reset(m, ctx->rbl, inf.data);
        }
    }

    return Roebling_Run(ctx->rbl);


    MemCtx_Free(m);
    return ret;
}
