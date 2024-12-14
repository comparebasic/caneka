/*
Main
Example and test runner

This file is mostly an example Caneka application, and it runs the tests
*/
#include <external.h>
#include <caneka.h>
#include <tests.h>
#include <example.h>

#define servecmd "serve"
#define testcmd "test"
#define transcmd "transp"

static status test(MemCtx *m, char *arg){
    status r = TEST_OK;
    word flags = 0;
    if(strncmp(arg, "test=no-color", strlen("test=no-color")) == 0){
        GLOBAL_flags |= NO_COLOR;
    }
    if(strncmp(arg, "test=html", strlen("test=html")) == 0){
        GLOBAL_flags |= HTML_OUTPUT;
    }
    Tests_Init(m);
    Test_Runner(m, "Caneka", Tests);
    return r;
}

status serve(MemCtx *m, char *arg){
    printf("ARg '%s;\n", arg);
    int l = strlen("serve=port:");
    int port = 0;
    if(strncmp(arg, "serve=port:", l) == 0){
        port = atoi(arg+l);
        printf("Serving on port: %d\n", port);
    }else{
        printf("No port specified: '%s'\n", arg);
        exit(1);
    }
    ProtoDef *def = HttpProtoDef_Make(m);
    def->getHandler = Example_getHandler;
    Serve *sctx = Serve_Make(m, def);
    def->source = (Abstract *)sctx;

    return Serve_Run(sctx, port);
}

status transpile(MemCtx *m, char *arg){
    Transp *ctx = CnkLangCtx_Make(m);
    ctx->src = String_Make(m, bytes("src"));
    ctx->dist = String_Make(m, bytes("dist"));
    Debug_Print((void *)ctx->src, 0, "[Transpiling  ", COLOR_BLUE, FALSE);
    Debug_Print((void *)ctx->dist, 0, "->", COLOR_BLUE, FALSE);
    printf("\x1b[%dm]\x1b[0m\n", COLOR_BLUE);
    return Transp_Trans(ctx);
}

static status handle(MemCtx *m, char *arg){
    int servecmd_l = strlen(servecmd);
    if(strncmp(arg, servecmd, strlen(servecmd)) == 0){
        return serve(m, arg);
    }else if(strncmp(arg, transcmd, strlen(servecmd)) == 0){
        return transpile(m, arg);
    }else if(strncmp(arg, testcmd, strlen(testcmd)) == 0){
        status r = test(m, arg);
        if(r == TEST_OK){
            return SUCCESS;
        }else{
            return r;
        }
    }

    return NOOP;
}

int main(int argc, char **argv){
    if(argc == 1){
        printf("%s [test,serve=port,trans]\n", argv[0]);
        exit(1);
    }

    MemCtx *m = MemCtx_Make();
    Caneka_Init(m);
    
    if(argc > 1){
        for(int i = 1; i < argc; i++){
            if(handle(m, argv[i]) == ERROR){
                exit(1);
            }
        }
    }

    return 0;
}
