#include <external.h>
#include <caneka.h>
#include <tests.h>
#include <proto_tests.h>

#define servecmd "serve="
#define testcmd "test"

static status test(MemCtx *m){
    status r = TEST_OK;
    r |= Core_Tests(m);
    /*
    r |= String_Tests(m);
    */
    r |= SpanInline_Tests(m);
    r |= Hash_Tests(m);
    r |= Table_Tests(m);
    r |= SpanSetup_Tests(m);
    r |= Span_Tests(m);
    r |= Match_Tests(m);
    /*
    r |= Roebling_Tests(m);
    r |= Serve_Tests(m);
    r |= ProtoHttp_Tests(m);
    r |= Xml_Tests(m);
    */
    return r;
}


static status handle(MemCtx *m, char *arg){
    int servecmd_l = strlen(servecmd);
    if(strncmp(arg, servecmd, strlen(servecmd)) == 0){
        /*
        arg += servecmd_l;
        printf("port is %s\n", arg);

        int port = atoi(arg);
        if(port == 0){
            Error("unable to find port for serve cmd");
            return ERROR;
        }

        ProtoDef *def = HttpProtoDef_Make(m);
        Table_Set(def->hdrHandlers_tbl_mk, (Abstract *)String_Make(m, bytes("Content-Length")),
            (Abstract *)Maker_Wrapped(m, Hdr_IntMk)); 
        Table_Set(def->hdrHandlers_tbl_mk, (Abstract *)String_Make(m, bytes("Cookie")),
            (Abstract *)Maker_Wrapped(m, hdrCookieProcess)); 

        Serve *sctx = Serve_Make(m, def);
        return Serve_Run(sctx, port);
        */
        return MISS;
    }else if(strncmp(arg, testcmd, strlen(testcmd)) == 0){
        status r = test(m);
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
        printf("%s [test,serve=port]\n", argv[0]);
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
