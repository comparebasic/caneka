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

static void *tests[] = {
    "Core", Core_Tests,
    "String", String_Tests,
    "SpanSetup", SpanSetup_Tests,
    "Span", Span_Tests,
    "SpanInline", SpanInline_Tests,
    "SpanClone", SpanClone_Tests,
    "Hash", Hash_Tests,
    "Table", Table_Tests,
    "TableResize", TableResize_Tests,
    "TablePreKey", TablePreKey_Tests,
    "Match", Match_Tests,
    "MatchElastic", MatchElastic_Tests,
    "Roebling", Roebling_Tests,
    "RoeblingRun", RoeblingRun_Tests,
    "RoeblingMark", RoeblingMark_Tests,
    "RoeblingStartSop", RoeblingStartStop_Tests,
    "XML", Xml_Tests,
    "XMLNested", XmlNested_Tests,
    "XMLParser", XmlParser_Tests,
    "Http", Http_Tests,
    "Queue", Queue_Tests,
    "QueueNext", QueueNext_Tests,
    "QueueMixed", QueueMixed_Tests,
    "Serve", Serve_Tests,
    "ServeHandle", ServeHandle_Tests,
    "ServeChunked", ServeChunked_Tests,
    NULL, NULL
};

static status test(MemCtx *m, char *arg){
    status r = TEST_OK;
    word flags = 0;
    if(strncmp(arg, "test=no-color", strlen("test=no-color")) == 0){
        GLOBAL_flags |= NO_COLOR;
    }
    Tests_Init(m);
    Test_Runner(m, "Caneka", tests);
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

static status handle(MemCtx *m, char *arg){
    int servecmd_l = strlen(servecmd);
    if(strncmp(arg, servecmd, strlen(servecmd)) == 0){
        return serve(m, arg);
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
        printf("%s [test,serve=port]\n", argv[0]);
        exit(1);
    }

    MemCtx *m = MemCtx_Make();
    SpanDef_Init();
    Span *p = Span_Make(m, TYPE_SPAN);
    String *s = String_Make(m, bytes("Hi"));
    Span_Set(p, 0, (Abstract *)s);

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
