#include <external.h>
#include <caneka.h>

static status Load_stats(Task *tsk, Step *st){
    ProtoCtx *proto = (ProtoCtx *)as(tsk->data, TYPE_PROTO_CTX);
    TcpCtx *tcp = (TcpCtx *)as(tsk->source, TYPE_TCP_CTX);
    HttpCtx *ctx = (HttpCtx *)as(proto->data, TYPE_HTTP_CTX);

    MemBookStats st;
    MemBook_GetStats(&st);

    Object *stats = Object_Make(m, ZERO);
    Object_Set(stats, Str_FromCstr(m, "uptime", ZERO),
        MicroTime_ToStr(m, ctx->metrics.start));
    Object *mem = Object_Make(m, ZERO);
    Object_Set(mem, Str_FromCstr(m, "mem-used", ZERO),
        Str_MemCount(m, st.pageIdx * PAGE_SIZE));
    Object_Set(mem, Str_FromCstr(m, "mem-total", ZERO),
        Str_MemCount(m, PAGE_COUNT * PAGE_SIZE));
    Object_Set(mem, Str_FromCstr(m, "mem-details", ZERO), Map_ToTable(m, &st));
    Object_Set(stats, Str_FromCstr(m, "mem", ZERO), mem);

    Object_Set(ctx->data, Str_FromCstr(m, "stats", ZERO), stats);

    st->type.state |= SUCCESS;
    return st->type.state;
}

static status routeInit(MemCh *m, TcpCtx *ctx){
    status r = READY;

    StrVec *path = StrVec_From(m, Str_CstrRef(m, "examples/web-server/pages/public"));
    StrVec *abs = IoUtil_AbsVec(m, path);
    ctx->pages = (Object *)Route_Make(m);
    r |= Route_Collect(ctx->pages, abs);

    StrVec *name = StrVec_From(m, Str_FromCstr(m, "/stats"));
    IoUtil_Annotate(m, name);

    Route *statHandler = Object_ByPath(ctx->pages, name, NULL, SPAN_OP_GET);
    Single *funcW = Ptr_Wrapped(m, Load_stats, TYPE_STEP_FUNC);
    Object_SetPropByIdx(statHandler, ROUTE_PROPIDX_ADD_STEP, funcW);

    StrVec *navPath = IoUtil_GetAbsVec(m,
        Str_FromCstr(m, "./examples/web-server/pages/nav.config", ZERO));
    ctx->nav = Nav_TableFromPath(m, ctx->pages, navPath);

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
