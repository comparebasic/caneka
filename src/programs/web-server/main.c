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
    Table_SetByCstr(mem, "mem-total", Str_MemCount(m, PAGE_COUNT * PAGE_SIZE));
    Table_SetByCstr(mem, "mem-details", Map_ToTable(m, &mst));
    
    Table_SetByCstr(stats, "mem", mem);
    Table_SetByCstr(ctx->data, "stats", stats);

    st->type.state |= SUCCESS;
    return st->type.state;
}

static status routeInit(MemCh *m, TcpCtx *ctx){
    status r = READY;

    ctx->pages = Route_Make(m);
    r |= Route_Collect(ctx->pages, IoAbsPath(m, "examples/web-server/pages/public"));

    StrVec *name = IoPath(m, "/stats");

    Route *statHandler = Inst_ByPath(ctx->pages, name, NULL, SPAN_OP_GET);
    Single *funcW = Ptr_Wrapped(m, Load_stats, TYPE_STEP_FUNC);
    Span_Set(statHandler, ROUTE_PROPIDX_ADD_STEP, funcW);

    StrVec *navPath = IoAbsPath(m,"examples/web-server/pages/nav.config");
    ctx->nav = Nav_TableFromPath(m, ctx->pages, navPath);

    ctx->inc = Route_Make(m);
    r |= Route_Collect(ctx->inc, IoAbsPath(m, "examples/web-server/pages/inc"));

    Route *stat = Route_From(m, IoAbsPath(m, "examples/web-server/pages/static"));
    Inst_ByPath(ctx->pages, IoPath(m, "/static/"), stat, SPAN_OP_SET);

    Route *sys = Route_From(m, IoAbsPath(m, "examples/web-server/pages/system"));
    Inst_ByPath(ctx->pages, IoPath(m, "/system/"), sys, SPAN_OP_SET);

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

    microTime now = MicroTime_Now();

    Str *help = S(m, "help");
    Str *noColor = S(m, "no-color");
    Str *log = S(m, "log");
    Str *foreground = S(m, "foreground");

    Table *resolveArgs = Table_Make(m);
    Args_Add(resolveArgs, help, NULL, ARG_OPTIONAL);
    Args_Add(resolveArgs, noColor, NULL, ARG_OPTIONAL);
    Args_Add(resolveArgs, S(m, "licence"), NULL, ARG_OPTIONAL);
    Args_Add(resolveArgs, S(m, "version"), NULL, ARG_OPTIONAL);
    Args_Add(resolveArgs, foreground, NULL, ARG_OPTIONAL);
    Args_Add(resolveArgs, log, NULL, ARG_OPTIONAL);

    StrVec *name = StrVec_From(m, S(m, argv[0]));
    IoUtil_Annotate(m, name);
    Str *fname = IoUtil_FnameStr(m, name);

    Table *cliArgs = Table_Make(m);
    CharPtr_ToTbl(m, resolveArgs, argc, argv, cliArgs);
    if(Table_GetHashed(cliArgs, help) != NULL){
        CharPtr_ToHelp(m, fname, resolveArgs, argc, argv);
        return 1;
    }

    if(Table_GetHashed(cliArgs, noColor) != NULL){
        Ansi_SetColor(FALSE);
    }

    if(Table_GetHashed(cliArgs, foreground) == NULL &&
            Table_GetHashed(cliArgs, log) != NULL){

        Str *logValue = Table_Get(cliArgs, log);

        Str *build = Str_Make(m, STR_DEFAULT);
        args[0] = logValue;
        args[1] = S(m, "_");
        args[2] = Str_FromI64(m, now);
        args[3] = S(m, ".out");
        args[4] = NULL;
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
        args[2] = Str_FromI64(m, now);
        args[3] = S(m, ".err");
        args[4] = NULL;
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
    }else if(Table_GetHashed(cliArgs, noColor) != NULL){
        Ansi_SetColor(FALSE);
    }

    util ip6[2] = {0, 0};
    Task *srv = WebServer_Make(3000, 0, ip6);
    routeInit(m, (TcpCtx *)srv->source);
    Task_Tumble(srv);

    DebugStack_Pop();
    close(2);
    close(1);
    exit(0);
    return 0;
}
