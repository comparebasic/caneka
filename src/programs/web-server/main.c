#include <external.h>
#include <caneka.h>

static status Load_stats(Step *st, Task *tsk){
    ProtoCtx *proto = (ProtoCtx *)as(tsk->data, TYPE_PROTO_CTX);
    TcpCtx *tcp = (TcpCtx *)as(tsk->source, TYPE_TCP_CTX);
    HttpCtx *ctx = (HttpCtx *)as(proto->data, TYPE_HTTP_CTX);

    MemCh *m = tsk->m;

    MemBookStats mst;
    MemBook_GetStats(m, &mst);

    Table *stats = Table_Make(m);
    Table_SetByCstr(stats, "uptime", MicroTime_ToStr(m, tcp->metrics.start));
    Table *mem = Table_Make(m);
    Table_SetByCstr(mem, "mem-used", Str_MemCount(m, mst.pageIdx * PAGE_SIZE));
    Table_Set(mem,  "mem-total", Str_MemCount(m, PAGE_COUNT * PAGE_SIZE));
    Table_Set(mem,  "mem-details", Map_ToTable(m, &mst));
    Table_Set(stats, "mem", mem);
    Table_Set(ctx->data, "stats", stats);

    st->type.state |= SUCCESS;
    return st->type.state;
}

static status routeInit(MemCh *m, TcpCtx *ctx){
    status r = READY;

    ctx->pages = Route_Make(m);
    r |= Route_Collect(ctx->pages, IoAbsPath(m, "examples/web-server/pages/public"));

    StrVec *name = IoPath(m, "/stats");

    Route *statHandler = NodeObj_ByPath(ctx->pages, name, NULL, SPAN_OP_GET);
    Single *funcW = Ptr_Wrapped(m, Load_stats, TYPE_STEP_FUNC);
    Span_Set(statHandler, ROUTE_PROPIDX_ADD_STEP, funcW);

    /*
    StrVec *navPath = IoAbsPath(m,"examples/web-server/pages/nav.config");
    ctx->nav = Nav_TableFromPath(m, ctx->pages, navPath);
    */

    ctx->inc = Route_Make(m);
    r |= Route_Collect(ctx->inc, IoAbsPath(m, "examples/web-server/pages/inc"));

    WebServer_AddRoute(m,
        ctx->pages,
        IoAbsPath(m, "examples/web-server/pages/static"),
        IoPath(m, "/static")
    );

    WebServer_AddRoute(m,
        ctx->pages,
        IoAbsPath(m, "examples/web-server/pages/system"),
        IoPath(m, "/system")
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

    Task *srv = WebServer_Make(3000, 0, ip6);
    routeInit(m, (TcpCtx *)srv->source);

    Task_Tumble(srv);

    DebugStack_Pop();
    return 0;
}
