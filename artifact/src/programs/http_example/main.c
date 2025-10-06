#include <external.h>
#include <caneka.h>
#include <http_example.h>

/* make a root handler and a maker for it to be the root of the chain for a Req
 * object created with the mk function of the serve ctx */


i32 main(int argc, char **argv){
    MemBook *cp = MemBook_Make(NULL);
    if(cp == NULL){
        Fatal(FUNCNAME, FILENAME, LINENUMBER, "MemBook created successfully", NULL);
    }

    MemCh *m = MemCh_Make();
    if(m == NULL){
        Fatal(FUNCNAME, FILENAME, LINENUMBER, "MemCh created successfully", NULL);
    }

    Caneka_Init(m);
    Lang_Init(m);
    Www_Init(m);
    DebugStack_Push(NULL, 0);

    if(argc == 2 && strncmp(argv[1], "--licence", strlen("--licence")) == 0){
        Show_Licences(OutStream);
        DebugStack_Pop();
        return 0;
    }

    TcpCtx *ctx = TcpCtx_Make(m, HttpE_Make, 8000, NULL, NULL);
    Req *root = ServeTcp_Make(ctx);

    test(m);

    DebugStack_Pop();
    return 0;
}
