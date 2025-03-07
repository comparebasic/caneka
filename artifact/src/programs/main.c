/*
Main
Example and test runner

This file is mostly an example Caneka application, and it runs the tests
*/
#include <external.h>
#include <caneka.h>
#include <tests/tests.h>

#define servecmd "serve"
#define testcmd "test"
#define transcmd "transp"
#define doccmd "doc"

static status test(MemCtx *m, char *arg){
    status r = READY;
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

static status debugFlag(char *arg){
    int color = 0;
    int l = strlen(arg);
    int l2 = 0;
    if(l < 3){
        return NOOP;
    }

    char *names[] = {
        "yellow", "red", "green", "cyan", "blue", "purple",   
    };

    int colors[] = {
        COLOR_YELLOW, COLOR_RED, COLOR_GREEN, COLOR_CYAN, 
        COLOR_BLUE, COLOR_PURPLE,
    };

    if(strncmp(arg, "d=", 2) == 0){
        arg += 2;
        l -= 2;
        for(int i = 0; i < 6; i++){
            l2 = strlen(names[i]);
            if(l >= l2 && strncmp(arg, names[i], l2) == 0){
                color = colors[i];
            }
        }
    }

    if(color != 0){
        DEBUG_PATMATCH = color;
        return SUCCESS;
    }

    return NOOP;
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

    return Serve_RunPort(sctx, port);
}

status transpile(MemCtx *m, char *arg){
    Transp *ctx = Transp_Make(m);
    ctx->src = String_Make(m, bytes("src"));
    ctx->dist = String_Make(m, bytes("dist/src"));
    Debug_Print((void *)ctx->src, 0, "[Transpiling  ", COLOR_BLUE, FALSE);
    Debug_Print((void *)ctx->dist, 0, "->", COLOR_BLUE, FALSE);
    int l = strlen("transp=cnk");
    if(strncmp(arg, "transp=cnk", l) == 0){
        CnkLang_Init(m);
        FmtCtx *cnkLang = CnkLangCtx_Make(m, (Abstract *)ctx);
        Table_Set(ctx->fmts, (Abstract *)String_Make(m, bytes(".cnk")), (Abstract *)cnkLang);
    }
    printf("\x1b[%dm]\x1b[0m\n", COLOR_BLUE);
    return Transp_Trans(ctx);
}

status doc(MemCtx *m, char *arg){
    Transp *ctx = Transp_Make(m);
    ctx->src = String_Make(m, bytes("dist/src/include"));
    ctx->dist = String_Make(m, bytes("dist/doc"));
    Debug_Print((void *)ctx->src, 0, "[Generating Documentation  ", COLOR_BLUE, FALSE);
    Debug_Print((void *)ctx->dist, 0, " ->  ", COLOR_BLUE, FALSE);
    printf("\x1b[%dm]\x1b[0m\n", COLOR_BLUE);
    FmtCtx *cdocLang = CdocCtx_Make(m, (Abstract *)ctx);
    
    if(DEBUG_CDOC){
        Stepper_Make(m, 
            (Abstract *)cdocLang->rbl, 
            ZERO,
            String_Make(m, bytes("")),
            String_Init(m, STRING_EXTEND));
    }

    Table_Set(ctx->fmts, (Abstract *)String_Make(m, bytes(".h")), (Abstract *)cdocLang);

    return Transp_Trans(ctx);
}

static status handle(MemCtx *m, char *arg){
    int servecmd_l = strlen(servecmd);
    if(strncmp(arg, servecmd, strlen(servecmd)) == 0){
        return serve(m, arg);
    }else if(strncmp(arg, transcmd, strlen(servecmd)) == 0){
        return transpile(m, arg);
    }else if(strncmp(arg, testcmd, strlen(testcmd)) == 0){
        return test(m, arg);
    }else if(strncmp(arg, doccmd, strlen(doccmd)) == 0){
        return doc(m, arg);
    }

    return NOOP;
}

void Cleanup(Abstract *m){
    return;
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
            if((debugFlag(argv[i]) & SUCCESS) != 0){
                continue;
            }
            if(handle(m, argv[i]) == ERROR){
                exit(1);
            }
        }
    }

    return 0;
}
