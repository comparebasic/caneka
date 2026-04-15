#include <external.h>
#include <caneka.h>

static status getDefaultData(MemCh *m, void *a, void *source){
    Table *data = (Table *)Ifc(m, a, TYPE_TABLE);
    TcpCtx *tcp = (TcpCtx *)Ifc(m, source, TYPE_TCP_CTX);
    Table *error = Table_Make(m);
    Table_Set(data, S(m, "nav"), tcp->nav);
    Table_Set(error, S(m, "name"), S(m, "None"));
    Table_Set(error, S(m, "details"), S(m, "None"));
    Table_Set(data, S(m, "error"), error);
    return SUCCESS;
}

static status Example_signup(Step *st, Task *tsk){
    ProtoCtx *proto = (ProtoCtx *)Ifc(tsk->m, tsk->data, TYPE_PROTO_CTX);
    TcpCtx *tcp = (TcpCtx *)Ifc(tsk->m, tsk->source, TYPE_TCP_CTX);
    HttpCtx *ctx = (HttpCtx *)Ifc(tsk->m, proto->ctx, TYPE_HTTP_CTX);

    MemCh *m = tsk->m;

    MemBookStats mst;
    MemBook_GetStats(m, &mst);

    Table *stats = Table_Make(m);
    Table_SetByCstr(stats, "uptime", Time_ToStr(m, &tcp->metrics.start));

    Table *mem = Table_Make(m);
    Table_SetByCstr(mem, "mem-used", Str_MemCount(m, mst.pageIdx * PAGE_SIZE));
    Table_SetByCstr(mem, "mem-total", Str_MemCount(m, PAGE_COUNT * PAGE_SIZE));
    Table_SetByCstr(mem, "mem-details", Map_ToTable(m, &mst));
    
    Table_SetByCstr(stats, "mem", mem);
    Table_SetByCstr(ctx->data, "stats", stats);

    st->type.state |= SUCCESS;
    return st->type.state;
}

static status Load_stats(Step *st, Task *tsk){
    ProtoCtx *proto = (ProtoCtx *)Ifc(tsk->m, tsk->data, TYPE_PROTO_CTX);
    TcpCtx *tcp = (TcpCtx *)Ifc(tsk->m, tsk->source, TYPE_TCP_CTX);
    HttpCtx *ctx = (HttpCtx *)Ifc(tsk->m, proto->ctx, TYPE_HTTP_CTX);

    MemCh *m = tsk->m;

    MemBookStats mst;
    MemBook_GetStats(m, &mst);

    Table *stats = Table_Make(m);
    Table_SetByCstr(stats, "uptime", Time_ToStr(m, &tcp->metrics.start));

    Table *mem = Table_Make(m);
    Table_SetByCstr(mem, "mem-used", Str_MemCount(m, mst.pageIdx * PAGE_SIZE));
    Table_SetByCstr(mem, "mem-total", Str_MemCount(m, PAGE_COUNT * PAGE_SIZE));
    Table_SetByCstr(mem, "mem-details", Map_ToTable(m, &mst));
    
    Table_SetByCstr(stats, "mem", mem);
    Table_SetByCstr(ctx->data, "stats", stats);

    st->type.state |= SUCCESS;
    return st->type.state;
}

static status routeInit(MemCh *m, TcpCtx *ctx){
    status r = READY;
    /*

    ctx->defaultData = getDefaultData;

    ctx->pages = Route_Make(m);
    r |= Route_Collect(ctx->pages, IoAbsPath(m, "fixtures/web-server/pages/public"));

    StrVec *name = IoPath(m, "/stats");

    Route *statHandler = Inst_ByPath(ctx->pages, name, NULL, SPAN_OP_GET);
    Single *funcW = Ptr_Wrapped(m, Load_stats, TYPE_STEP_FUNC);
    Span_Set(statHandler, ROUTE_PROPIDX_ADD_STEP, funcW);

    StrVec *navPath = IoAbsPath(m,"fixtures/web-server/pages/nav.config");
    ctx->nav = Nav_TableFromPath(m, ctx->pages, navPath);

    ctx->inc = Route_Make(m);
    r |= Route_Collect(ctx->inc, IoAbsPath(m, "fixtures/web-server/pages/inc"));

    Route *stat = Route_From(m, IoAbsPath(m, "fixtures/web-server/pages/static"));
    Inst_ByPath(ctx->pages, IoPath(m, "/static/"), stat, SPAN_OP_SET);

    Route *forms = Route_From(m, IoAbsPath(m, "fixtures/web-server/pages/forms"));
    Inst_ByPath(ctx->pages, IoPath(m, "/forms/"), forms, SPAN_OP_SET);

    Route *sys = Route_From(m, IoAbsPath(m, "fixtures/web-server/pages/system"));
    Inst_ByPath(ctx->pages, IoPath(m, "/system/"), sys, SPAN_OP_SET);

    */
    return r;
}

static status run(MemCh *m, void *tsk, void *_source){
    DebugStack_Push(NULL, ZERO);
    DebugStack_SetRef(tsk, ((Abstract *)tsk)->type.of);
    status r = Task_Tumble((Task *)tsk);
    DebugStack_Pop();
    return r;
}

i32 main(int argc, char **argv){
    MemBook *cp = MemBook_Make(NULL);
    void *args[8];
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
    Core_Direct(m, 1, 2);

    struct timespec now;
    Time_Now(&now);

    Str *helpKey = S(m, "help");
    Str *noColorKey = S(m, "no-color");
    Str *logKey = S(m, "log");
    Str *foregroundKey = S(m, "foreground");
    Str *configKey = S(m, "config");
    Str *licenceKey = S(m, "licence");
    Str *versionKey = S(m, "licence");

    CliArgs *cli = CliArgs_Make(argc, argv);

    Args_Add(cli, helpKey, NULL, ARG_OPTIONAL,
        Sv(m, "Show this help menu."));
    Args_Add(cli, noColorKey, NULL, ARG_OPTIONAL,
        Sv(m, "Skip ansi color sequences in output."));
    Args_Add(cli, configKey, NULL, ZERO,
        Sv(m, "Config to use with defined routes and auth details."));
    Args_Add(cli, licenceKey, NULL, ARG_OPTIONAL,
        Sv(m, "Show the licences used in this software"));
    Args_Add(cli, versionKey, NULL, ARG_OPTIONAL,
        Sv(m, "Show the version of this software"));
    Args_Add(cli, foregroundKey, NULL, ARG_OPTIONAL,
        Sv(m, "Show messages to the console."));
    Args_Add(cli, logKey, NULL, ARG_OPTIONAL,
        Sv(m, "Direct logs to a file starting with this name."));

    CliArgs_Parse(cli);

    if(CliArgs_Get(cli, helpKey) != NULL){
        CharPtr_ToHelp(cli);
        return 1;
    }

    if(CliArgs_Get(cli, noColorKey) != NULL){
        Ansi_SetColor(FALSE);
    }

    StrVec *configPath = CliArgs_GetAbsPath(cli, configKey);
    StrVec *configDir = IoUtil_BasePath(m, configPath);

    NodeObj *config = Config_FromPath(m, StrVec_Str(m, configPath));
    if(config == NULL){
        void *args[] = {configPath, NULL};
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "Config file not found @", args);
        exit(13);
        return 13;
    }

    if(CliArgs_Get(cli, foregroundKey) == NULL &&
            CliArgs_Get(cli, logKey) != NULL){

        Str *logValue = CliArgs_Get(cli, logKey);

        Str *build = Str_Make(m, STR_DEFAULT);
        args[0] = logValue;
        args[1] = S(m, "_");
        args[2] = Str_FromI64(m, now.tv_sec);
        args[3] = S(m, "-");
        args[4] = Str_FromI64(m, now.tv_nsec);
        args[5] = S(m, ".out");
        args[6] = NULL;
        Str_AddChain(m, build, args);

        Str *logPath = IoUtil_GetAbsPath(m, build);
        Buff *bf = Buff_Make(m, ZERO);
        File_Open(bf, logPath, O_WRONLY|O_CREAT|O_APPEND);
        i32 outFd = 1;
        if(bf->fd > 0){
            outFd = bf->fd;
        }

        build = Str_Make(m, STR_DEFAULT);
        args[0] = logValue;
        args[1] = S(m, "_");
        args[2] = Str_FromI64(m, now.tv_sec);
        args[3] = S(m, "-");
        args[4] = Str_FromI64(m, now.tv_nsec);
        args[5] = S(m, ".err");
        args[6] = NULL;
        Str_AddChain(m, build, args);

        Str *errPath = IoUtil_GetAbsPath(m, build);
        bf = Buff_Make(m, ZERO);
        File_Open(bf, errPath, O_WRONLY|O_CREAT|O_APPEND);
        i32 errFd = 1;
        if(bf->fd > 0){
            errFd = bf->fd;
        }

        args[0] = logPath;
        args[1] = errPath;
        args[2] = NULL;
        Out("^p.Redirecting all output to $ and $^0\n", args);

        Ansi_SetColor(FALSE);
        Core_Direct(m, outFd, errFd);
    }else if(CliArgs_Get(cli, noColorKey) != NULL){
        Ansi_SetColor(FALSE);
    }

    util ip6[2] = {0, 0};
    Task *srv = WebServer_Make(3000, 0, ip6);
    WebServer_SetConfig(srv, configDir, config, NULL);
    exit(1);

    routeInit(m, (TcpCtx *)srv->source);
    Task_Tumble(srv);

    DebugStack_Pop();
    close(2);
    close(1);
    exit(0);
    return 0;
}
