/*
Main
Example and test runner

This file is mostly an example Caneka application, and it runs the tests
*/
#include <external.h>
#include <caneka.h>
#include <tests.h>
#include <proto_tests.h>

#define servecmd "serve="
#define testcmd "test"

static void *tests[] = {
    "Core", Core_Tests,
    "String", String_Tests,
    "SpanSetup", SpanSetup_Tests,
    "Span", Span_Tests,
    "SpanInline", SpanInline_Tests,
    "Hash", Hash_Tests,
    "Table", Table_Tests,
    "TableResize", TableResize_Tests,
    "TablePreKey", TablePreKey_Tests,
    "Match", Match_Tests,
    "MatchElastic", MatchElastic_Tests,
    "Roebling", Roebling_Tests,
    "RoeblingRun", RoeblingRun_Tests,
    "RoeblingMark", RoeblingMark_Tests,
    "XML", Xml_Tests,
    /*
    "XMLNested", XmlNested_Tests,
    */
    NULL, NULL
    /*
    Serve_Tests,
    ProtoHttp_Tests,
    */
};

static status test(MemCtx *m){
    status r = TEST_OK;
    Test_Runner(m, "Caneka", tests);
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
