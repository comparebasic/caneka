#include <external.h>
#include <caneka.h>

struct span *RouteFuncTable = NULL;
struct span *RouteMimeTable = NULL;

static status file(MemCh *m, Str *path, Str *file, Abstract *source){
    Abstract *args[5];
    RouteCtx *rctx = (RouteCtx *)source;

    StrVec *abs = StrVec_From(m, path);
    Path_AddSlash(m, abs);
    StrVec_Add(abs, file);
    IoUtil_Annotate(m, abs);

    StrVec *name = Path_Name(m, abs);
    StrVec *ext = Path_Ext(m, abs);

    Str *mime = (Str *)Table_Get(RouteMimeTable, (Abstract *)ext);
    Single *funcW = (Single *)Table_Get(RouteFuncTable, (Abstract *)ext);

    if(mime == NULL || funcW == NULL){
        Abstract *args[] = {
            (Abstract *)mime,
            (Abstract *)ext,
            (Abstract *)path,
            (Abstract *)file,
            (Abstract *)RouteMimeTable,
            NULL
        };
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "Mime & not found for this file with ext:@ path:@ file:@ mimeTable:@", args);
        return ERROR;
    }

    if(funcW->type.state & ROUTE_FORBIDDEN){
        return NOOP;
    }

    StrVec *pathV = StrVec_From(m, path);
    IoUtil_Annotate(m, pathV);
    StrVec *objPath = Path_SubClone(m, pathV, rctx->path->p->max_idx+1);
    objPath->type.state |= STRVEC_PATH;

    Str *index = Str_FromCstr(m, "index", ZERO);
    if(!Equals((Abstract *)name, (Abstract *)index)){
        if(funcW->type.state & (ROUTE_DYNAMIC|ROUTE_PAGE)){
            Path_Add(m, objPath, name);
        }else{
            Path_AddStr(objPath, file);
        }
    }

    Route *subRt = Object_ByPath(rctx->root, objPath, NULL, SPAN_OP_RESERVE);

    Object_SetPropByIdx(subRt, ROUTE_PROPIDX_PATH, (Abstract *)objPath);
    Object_SetPropByIdx(subRt, ROUTE_PROPIDX_FILE, (Abstract *)abs);
    Object_SetPropByIdx(subRt, ROUTE_PROPIDX_FUNC, (Abstract *)funcW);
    Object_SetPropByIdx(subRt, ROUTE_PROPIDX_MIME, (Abstract *)mime);
    Object_SetPropByIdx(subRt, ROUTE_PROPIDX_TYPE, (Abstract *)ext);
    subRt->type.state |= funcW->type.state;

    Route_Prepare(subRt, rctx);

    return SUCCESS;
}

static status routeFuncStatic(Buff *bf, Abstract *action, Object *_data, Abstract *_source){
    Str *pathS = StrVec_Str(bf->m, (StrVec *)as(action, TYPE_STRVEC));
    bf->type.state |= BUFF_UNBUFFERED;
    return File_Open(bf, pathS, O_RDONLY);
}

static status routeFuncTempl(Buff *bf, Abstract *action, Object *data, Abstract *source){
    Templ *templ = (Templ *)as(action, TYPE_TEMPL);
    status r = Templ_ToS(templ, bf, (Abstract *)data, source);
    Templ_Reset(templ);
    return r;
}

static status routeFuncFmt(Buff *bf, Abstract *action, Object *_data, Abstract *source){
    return Fmt_ToHtml(bf, (Mess *)as(action, TYPE_MESS));
}

static status routeFuncFileDb(Buff *bf, Abstract *action, Object *data, Abstract *source){
    StrVec *path = StrVec_From(bf->m, Str_FromCstr(bf->m, "filedb.keys", ZERO));
    Path_DotAnnotate(bf->m, path);
    Table *keys = (Table *)Object_ByPath(data, path, NULL, SPAN_OP_GET);

    FileDB *fdb = (FileDB *)as(action, TYPE_FILEDB);
    Table *tbl = FileDB_ToTbl(fdb, keys);

    /* tbl to json */
    return NOOP;
}

Route *Route_GetNav(Route *rt){
    MemCh *m = Object_GetMem(rt);
    Route *nav = Route_Make(m);
    return nav;
}

status Route_Prepare(Route *rt, RouteCtx *ctx){
    DebugStack_Push(rt, rt->type.of);
    MemCh *m = Object_GetMem(rt);
    Abstract *args[3];

    StrVec *path = (StrVec *)Object_GetPropByIdx(rt, ROUTE_PROPIDX_FILE);
    StrVec *type = (StrVec *)Object_GetPropByIdx(rt, ROUTE_PROPIDX_TYPE);
    Single *funcW = (Single *)as(
        Object_GetPropByIdx(rt, ROUTE_PROPIDX_FUNC),
        TYPE_WRAPPED_FUNC
    );

    if(funcW == NULL || (funcW->type.state & ROUTE_STATIC)){
        Object_SetPropByIdx(rt, ROUTE_PROPIDX_ACTION, (Abstract *)path);
    }

    Str *pathS = StrVec_Str(m, path);
    if(funcW->type.state & ROUTE_FMT){
        StrVec *content = File_ToVec(m, pathS);
        Cursor *curs = Cursor_Make(m, content); 
        Roebling *rbl = FormatFmt_Make(m, curs, NULL);
        Roebling_Run(rbl);
        if(rbl->type.state & ERROR){
            args[0] = (Abstract *)path;
            args[1] = NULL;
            Error(m, FUNCNAME, FILENAME, LINENUMBER,
                "Error preparing template for $", args);
            rt->type.state |= ERROR;
            DebugStack_Pop();
            return rt->type.state;
        }

        Object_SetPropByIdx(rt, ROUTE_PROPIDX_ACTION, rbl->dest);
        DebugStack_Pop();
        return SUCCESS;
    }else if(funcW->type.state & ROUTE_DYNAMIC){
        StrVec *content = File_ToVec(m, pathS);

        if(content == NULL || content->total == 0){
            args[0] = (Abstract *)path;
            args[1] = (Abstract *)content;
            args[2] = NULL;
            Error(m, FUNCNAME, FILENAME, LINENUMBER,
                "Error path for Templ has no content for: $ -> @", args);
            rt->type.state |= ERROR;
            DebugStack_Pop();
            return rt->type.state;
        }

        StrVec *config = StrVec_Make(m);
        StrVec_AddVec(config, ctx->path);
        StrVec_AddVec(config, (StrVec *)Object_GetPropByIdx(rt, ROUTE_PROPIDX_PATH));

        Str *configS = StrVec_StrCombo(m,
            (Abstract *)config, (Abstract *)Str_FromCstr(m, ".config", ZERO));

        Buff *bf = Buff_Make(m, ZERO);
        Object *data = Config_FromPath(m, configS);
        if(data != NULL){
            Object_SetPropByIdx(rt, ROUTE_PROPIDX_DATA, (Abstract *)data);
        }

        Cursor *curs = Cursor_Make(m, content);
        TemplCtx *ctx = TemplCtx_FromCurs(m, curs, NULL);

        Templ *templ = (Templ *)Templ_Make(m, ctx->it.p);
        if((Templ_Prepare(templ) & PROCESSING) == 0){
            args[0] = (Abstract *)path;
            args[1] = NULL;
            Error(m, FUNCNAME, FILENAME, LINENUMBER,
                "Error preparing template for $", args);
            rt->type.state |= ERROR;
            DebugStack_Pop();
            return rt->type.state;
        }

        Object_SetPropByIdx(rt, ROUTE_PROPIDX_ACTION, (Abstract *)templ);
        DebugStack_Pop();
        return SUCCESS;
    }else if(funcW->type.state & ROUTE_FILEDB){
        FileDB *fdb = FileDB_Make(m, pathS);
        Object_SetPropByIdx(rt, ROUTE_PROPIDX_ACTION, (Abstract *)fdb);
        DebugStack_Pop();
        return SUCCESS;
    }

    DebugStack_Pop();
    return NOOP;
}

status Route_Handle(Route *rt, Buff *bf, Object *data, Abstract *source){
    DebugStack_Push(rt, rt->type.of);
    Abstract *action = Object_GetPropByIdx(rt, ROUTE_PROPIDX_ACTION);
    Abstract *config = Object_GetPropByIdx(rt, ROUTE_PROPIDX_DATA);
    if(config != NULL){
        Object_Set(data, (Abstract *)Str_FromCstr(bf->m, "config", STRING_COPY), config);
    }

    Single *funcW = (Single *)as(
        Object_GetPropByIdx(rt, ROUTE_PROPIDX_FUNC),
        TYPE_WRAPPED_FUNC
    );
    RouteFunc func = (RouteFunc)funcW->val.ptr;
    return func(bf, action, data, source);
}

status Route_Collect(Route *rt, StrVec *path){
    MemCh *m = Object_GetMem(rt);
    IoUtil_Annotate(m, path);

    StrVec *root = StrVec_From(m, Str_CstrRef(m, "/"));
    IoUtil_Annotate(m, root);
    Object_SetPropByIdx(rt, ROUTE_PROPIDX_PATH, (Abstract *)root);

    RouteCtx ctx;
    ctx.type.of = TYPE_ROUTE_CTX;
    ctx.type.state = ZERO;
    ctx.root = rt;
    ctx.path = path;

    return Dir_Climb(m, StrVec_Str(m, path), NULL, file, (Abstract *)&ctx);
}

Route *Route_Make(MemCh *m){
    return Object_Make(m, TYPE_WWW_ROUTE);
}

status Route_ClsInit(MemCh *m){
    status r = READY;
    ClassDef *cls = ClassDef_Make(m);
    cls->objType.of = TYPE_WWW_ROUTE;
    cls->name = Str_CstrRef(m, "Route");
    Class_SetupProp(cls, Str_CstrRef(m, "path"));
    Class_SetupProp(cls, Str_CstrRef(m, "file"));
    Class_SetupProp(cls, Str_CstrRef(m, "func"));
    Class_SetupProp(cls, Str_CstrRef(m, "mime"));
    Class_SetupProp(cls, Str_CstrRef(m, "type"));
    Class_SetupProp(cls, Str_CstrRef(m, "action"));
    Class_SetupProp(cls, Str_CstrRef(m, "data"));
    r |= Class_Register(m, cls);

    if(RouteFuncTable == NULL){
        RouteFuncTable = Table_Make(m);
        RouteMimeTable = Table_Make(m);

        Str *key = Str_CstrRef(m, "html");
        Single *funcW = Func_Wrapped(m, routeFuncStatic);
        funcW->type.state |= (ROUTE_PAGE|ROUTE_STATIC);
        Table_Set(RouteFuncTable, (Abstract *)key, (Abstract *)funcW);
        Table_Set(RouteMimeTable,
            (Abstract *)key, (Abstract *)Str_CstrRef(m, "text/html"));

        key = Str_CstrRef(m, "png");
        funcW = Func_Wrapped(m, routeFuncStatic);
        funcW->type.state |= (ROUTE_ASSET|ROUTE_STATIC);
        Table_Set(RouteFuncTable, (Abstract *)key, (Abstract *)funcW);
        Table_Set(RouteMimeTable,
            (Abstract *)key, (Abstract *)Str_CstrRef(m, "image/png"));

        key = Str_CstrRef(m, "jpg");
        Table_Set(RouteFuncTable, (Abstract *)key, (Abstract *)funcW);
        Table_Set(RouteMimeTable,
            (Abstract *)key, (Abstract *)Str_CstrRef(m, "image/jpeg"));

        key = Str_CstrRef(m, "js");
        funcW = Func_Wrapped(m, routeFuncStatic);
        funcW->type.state |= (ROUTE_SCRIPT|ROUTE_STATIC);
        Table_Set(RouteFuncTable, (Abstract *)key, (Abstract *)funcW);
        Table_Set(RouteMimeTable,
            (Abstract *)key, (Abstract *)Str_CstrRef(m, "text/javascript"));

        key = Str_CstrRef(m, "css");
        funcW = Func_Wrapped(m, routeFuncStatic);
        funcW->type.state |= (ROUTE_STATIC);
        Table_Set(RouteFuncTable, (Abstract *)key, (Abstract *)funcW);
        Table_Set(RouteMimeTable,
            (Abstract *)key, (Abstract *)Str_CstrRef(m, "text/css"));

        key = Str_CstrRef(m, "txt");
        funcW = Func_Wrapped(m, routeFuncStatic);
        funcW->type.state |= ROUTE_STATIC;
        Table_Set(RouteFuncTable, (Abstract *)key, (Abstract *)funcW);
        Table_Set(RouteMimeTable,
            (Abstract *)key, (Abstract *)Str_CstrRef(m, "text/plain"));

        key = Str_CstrRef(m, "fmt");
        funcW = Func_Wrapped(m, routeFuncFmt);
        funcW->type.state |= ROUTE_FMT;
        Table_Set(RouteFuncTable, (Abstract *)key, (Abstract *)funcW);
        Table_Set(RouteMimeTable,
            (Abstract *)key, (Abstract *)Str_CstrRef(m, "text/html"));

        key = Str_CstrRef(m, "d");
        funcW = Func_Wrapped(m, routeFuncFileDb);
        funcW->type.state |= ROUTE_FILEDB;
        Table_Set(RouteFuncTable, (Abstract *)key, (Abstract *)funcW);
        Table_Set(RouteMimeTable,
            (Abstract *)key, (Abstract *)Str_CstrRef(m, "application/json"));

        key = Str_CstrRef(m, "templ");
        funcW = Func_Wrapped(m, routeFuncTempl);
        funcW->type.state |= ROUTE_DYNAMIC;
        Table_Set(RouteFuncTable, (Abstract *)key, (Abstract *)funcW);
        Table_Set(RouteMimeTable,
            (Abstract *)key, (Abstract *)Str_CstrRef(m, "text/html"));

        key = Str_CstrRef(m, "config");
        funcW = Func_Wrapped(m, NULL);
        funcW->type.state |= ROUTE_FORBIDDEN;
        Table_Set(RouteFuncTable, (Abstract *)key, (Abstract *)funcW);
        Table_Set(RouteMimeTable,
            (Abstract *)key, (Abstract *)Str_CstrRef(m, "text/plain"));
    }

    return r;
}
