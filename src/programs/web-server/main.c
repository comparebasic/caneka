#include <external.h>
#include <caneka.h>

static status routeInit(MemCh *m, TcpCtx *ctx){
    status r = READY;

    StrVec *path = StrVec_From(m, Str_CstrRef(m, "examples/web-server/pages/public"));
    StrVec *abs = IoUtil_AbsVec(m, path);
    ctx->nav = (Object *)Route_Make(m);
    r |= Route_Collect(ctx->nav, abs);

    ctx->pages = (Object *)Route_Make(m);
    Object_Merge(ctx->pages, ctx->nav, ctx->nav->objType.of);

    path = StrVec_From(m, Str_CstrRef(m, "examples/web-server/pages/inc"));
    abs = IoUtil_AbsVec(m, path);
    ctx->inc = (Object *)Route_Make(m);
    r |= Route_Collect(ctx->inc, abs);

    addRoute(m,
        ctx->pages,
        Str_FromCstr(m, "examples/web-server/pages/static", STRING_COPY),
        Str_FromCstr(m, "/static", STRING_COPY)
    );

    addRoute(m,
        ctx->pages,
        Str_FromCstr(m, "examples/web-server/pages/system", STRING_COPY),
        Str_FromCstr(m, "/system", STRING_COPY)
    );

    return r;
}


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
    Inter_Init(m);
    DebugStack_Push(NULL, 0);

    if(argc == 2 && strncmp(argv[1], "--licence", strlen("--licence")) == 0){
        Show_Licences(OutStream);
        DebugStack_Pop();
        return 0;
    }

    util ip6[2] = {0, 0};
    StrVec *path = IoUtil_GetAbsVec(m, Str_CstrRef(m, "examples/web-server"));

    Task *srv = WebServer_Make(path, 3000, 0, ip6);
    routeInit(m, (TcpCtx *)srv->source);

    Task_Tumble(tsk);

    DebugStack_Pop();
    return 0;
}
