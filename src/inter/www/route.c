#include <external.h>
#include <caneka.h>

struct span *RouteFuncTable = NULL;
struct span *RouteMimeTable = NULL;

static status fileFunc(MemCh *m, Str *path, Str *file, void *source){
    void *args[5];
    RouteCtx *rctx = (RouteCtx *)source;

    StrVec *abs = StrVec_From(m, path);
    Path_AddSlash(m, abs);
    StrVec_Add(abs, file);
    IoUtil_Annotate(m, abs);

    StrVec *name = Path_Name(m, abs);
    StrVec *ext = Path_Ext(m, abs);

    Str *mime = (Str *)Table_Get(RouteMimeTable, ext);
    Single *funcW = (Single *)Table_Get(RouteFuncTable, ext);

    if(mime == NULL || funcW == NULL){
        void *args[] = {
            mime,
            ext,
            path,
            file,
            RouteMimeTable,
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
    word flags = ZERO;
    if(!Equals(name, index)){
        if((funcW->type.state & ROUTE_ASSET) == 0){
            Path_Add(m, objPath, name);
        }else{
            Path_AddStr(objPath, file);
        }
    }else{
        flags |= ROUTE_INDEX;
    }

    Route *subRt = Object_ByPath(rctx->root, objPath, NULL, SPAN_OP_RESERVE);

    Object_SetPropByIdx(subRt, ROUTE_PROPIDX_PATH, objPath);
    Object_SetPropByIdx(subRt, ROUTE_PROPIDX_FILE, abs);
    Object_SetPropByIdx(subRt, ROUTE_PROPIDX_FUNC, funcW);
    Object_SetPropByIdx(subRt, ROUTE_PROPIDX_MIME, mime);
    Object_SetPropByIdx(subRt, ROUTE_PROPIDX_TYPE, ext);
    subRt->type.state |= (funcW->type.state|flags);

    Route_Prepare(subRt, rctx);

    return SUCCESS;
}

static status routeFuncStatic(Buff *bf, void *action, Object *_data, void *_source){
    Str *pathS = StrVec_Str(bf->m, (StrVec *)as(action, TYPE_STRVEC));
    bf->type.state |= BUFF_UNBUFFERED;
    return File_Open(bf, pathS, O_RDONLY);
}

static status routeFuncTempl(Buff *bf, void *action, Object *data, void *source){
    Templ *templ = (Templ *)as(action, TYPE_TEMPL);

    Templ_Reset(templ);
    templ->type.state |= bf->type.state;
    status r = Templ_ToS(templ, bf, data, source);
    return r;
}

static status routeFuncFmt(Buff *bf, void *action, Object *_data, void *source){
    return Fmt_ToHtml(bf, (Mess *)as(action, TYPE_MESS));
}

static status routeFuncFileDb(Buff *bf, void *action, Object *data, void *source){
    StrVec *path = StrVec_From(bf->m, Str_FromCstr(bf->m, "filedb.keys", ZERO));
    Path_DotAnnotate(bf->m, path);
    Table *keys = (Table *)Object_ByPath(data, path, NULL, SPAN_OP_GET);

    FileDB *fdb = (FileDB *)as(action, TYPE_FILEDB);
    Table *tbl = FileDB_ToTbl(fdb, keys);

    /* tbl to json */
    return NOOP;
}

static status Route_addConfigData(RouteCtx *ctx, Route *rt, StrVec *token){
    MemCh *m = Object_GetMem(rt);
    void *args[3];
    StrVec *config = StrVec_Make(m);
    StrVec_AddVec(config, ctx->path);
    StrVec_AddVec(config, (StrVec *)token);

    Str *configS = StrVec_StrCombo(m,
        config, Str_FromCstr(m, ".config", ZERO));

    Object *data = Config_FromPath(m, configS);
    if(data != NULL){
        Object_SetPropByIdx(rt, ROUTE_PROPIDX_DATA, data);
        return SUCCESS;
    }
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
    void *args[3];

    StrVec *path = (StrVec *)Object_GetPropByIdx(rt, ROUTE_PROPIDX_FILE);
    StrVec *type = (StrVec *)Object_GetPropByIdx(rt, ROUTE_PROPIDX_TYPE);
    StrVec *token = StrVec_Make(m);
    StrVec_AddVec(token, (StrVec *)Object_GetPropByIdx(rt, ROUTE_PROPIDX_PATH));
    if(rt->type.state & ROUTE_INDEX){
        StrVec_Add(token, Str_FromCstr(m, "index", ZERO));
    }
    Single *funcW = (Single *)as(
        Object_GetPropByIdx(rt, ROUTE_PROPIDX_FUNC),
        TYPE_WRAPPED_FUNC
    );

    if(funcW == NULL || (funcW->type.state & ROUTE_STATIC)){
        Object_SetPropByIdx(rt, ROUTE_PROPIDX_ACTION, path);
    }

    Route_addConfigData(ctx, rt, token);

    Str *pathS = StrVec_Str(m, path);
    if(funcW->type.state & ROUTE_FMT){
        StrVec *content = File_ToVec(m, pathS);
        Cursor *curs = Cursor_Make(m, content); 
        Roebling *rbl = FormatFmt_Make(m, curs, NULL);
        Roebling_Run(rbl);
        if(rbl->type.state & ERROR){
            args[0] = path;
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
            args[0] = path;
            args[1] = content;
            args[2] = NULL;
            Error(m, FUNCNAME, FILENAME, LINENUMBER,
                "Error path for Templ has no content for: $ -> @", args);
            rt->type.state |= ERROR;
            DebugStack_Pop();
            return rt->type.state;
        }

        Buff *bf = Buff_Make(m, ZERO);

        Cursor *curs = Cursor_Make(m, content);
        TemplCtx *ctx = TemplCtx_FromCurs(m, curs, NULL);

        Templ *templ = (Templ *)Templ_Make(m, ctx->it.p);
        if((Templ_Prepare(templ) & PROCESSING) == 0){
            args[0] = path;
            args[1] = NULL;
            Error(m, FUNCNAME, FILENAME, LINENUMBER,
                "Error preparing template for $", args);
            rt->type.state |= ERROR;
            Templ_Reset(templ);
            DebugStack_Pop();
            return rt->type.state;
        }

        Object_SetPropByIdx(rt, ROUTE_PROPIDX_ACTION, templ);
        DebugStack_Pop();
        return SUCCESS;
    }else if(funcW->type.state & ROUTE_FILEDB){
        FileDB *fdb = FileDB_Make(m, pathS);
        Object_SetPropByIdx(rt, ROUTE_PROPIDX_ACTION, fdb);
        DebugStack_Pop();
        return SUCCESS;
    }

    DebugStack_Pop();
    return NOOP;
}

Route *Route_GetHandler(Route *rt, StrVec *_path){
    MemCh *m = Object_GetMem(rt);
    StrVec *ext = Path_Ext(m, _path);
    Single *funcW = (Single *)Table_Get(RouteFuncTable, ext);

    StrVec *path = NULL;
    if(funcW != NULL){
        if(funcW->type.state & ROUTE_ASSET){
            path = Path_ReJoinExt(m, _path);
        }else{
            path = Path_WithoutExt(m, _path);
        }
    }else{
        path = _path;
    }

    Route *handler = (Route *)Object_ByPath(rt, path, NULL, SPAN_OP_GET);

    return handler;
}

status Route_Handle(Route *rt, Buff *bf, Object *data, void *source){
    DebugStack_Push(rt, rt->type.of);
    StrVec *path = (StrVec *)Object_GetPropByIdx(rt, ROUTE_PROPIDX_FILE);

    word fl = bf->m->type.state;
    bf->m->type.state |= MEMCH_BASE;
    DebugStack_SetRef(StrVec_Str(bf->m, path), TYPE_STR);
    bf->m->type.state = fl;

    Abstract *action = Object_GetPropByIdx(rt, ROUTE_PROPIDX_ACTION);
    Abstract *config = Object_GetPropByIdx(rt, ROUTE_PROPIDX_DATA);
    if(config != NULL){
        Object_Set(data, Str_FromCstr(bf->m, "config", STRING_COPY), config);
    }

    Single *funcW = (Single *)as(
        Object_GetPropByIdx(rt, ROUTE_PROPIDX_FUNC),
        TYPE_WRAPPED_FUNC
    );

    RouteFunc func = (RouteFunc)funcW->val.ptr;
    status r = func(bf, action, data, source);

    DebugStack_Pop();
    return r;
}

status Route_Collect(Route *rt, StrVec *path){
    MemCh *m = Object_GetMem(rt);
    IoUtil_Annotate(m, path);

    StrVec *root = StrVec_From(m, Str_CstrRef(m, "/"));
    IoUtil_Annotate(m, root);
    Object_SetPropByIdx(rt, ROUTE_PROPIDX_PATH, root);

    RouteCtx ctx;
    ctx.type.of = TYPE_ROUTE_CTX;
    ctx.type.state = ZERO;
    ctx.root = rt;
    ctx.path = path;

    return Dir_Climb(m, StrVec_Str(m, path), NULL, fileFunc, &ctx);
}

Route *Route_Make(MemCh *m){
    return Object_Make(m, TYPE_WWW_ROUTE);
}

Route *Route_From(MemCh *m, StrVec *dir){
    Route *rt = (Object *)Route_Make(m);
    Route_Collect(rt, dir);
    return rt;
}

status Route_ClsInit(MemCh *m){
    status r = READY;
    ClassDef *cls = ClassDef_Make(m);
    cls->type.of = TYPE_WWW_ROUTE;
    cls->name = Str_CstrRef(m, "Route");
    cls->api.toS = Object_Print;
    Class_SetupProp(cls, Str_CstrRef(m, "path"));
    Class_SetupProp(cls, Str_CstrRef(m, "file"));
    Class_SetupProp(cls, Str_CstrRef(m, "func"));
    Class_SetupProp(cls, Str_CstrRef(m, "mime"));
    Class_SetupProp(cls, Str_CstrRef(m, "type"));
    Class_SetupProp(cls, Str_CstrRef(m, "action"));
    Class_SetupProp(cls, Str_CstrRef(m, "data"));
    Class_SetupProp(cls, Str_CstrRef(m, "addStep"));
    r |= Class_Register(m, cls);

    if(RouteFuncTable == NULL){
        RouteFuncTable = Table_Make(m);
        RouteMimeTable = Table_Make(m);

        Str *key = Str_CstrRef(m, "html");
        Single *funcW = Func_Wrapped(m, routeFuncStatic);
        funcW->type.state |= (ROUTE_STATIC);
        Table_Set(RouteFuncTable, key, funcW);
        Table_Set(RouteMimeTable,
            key, Str_CstrRef(m, "text/html"));

        key = Str_CstrRef(m, "png");
        funcW = Func_Wrapped(m, routeFuncStatic);
        funcW->type.state |= (ROUTE_ASSET|ROUTE_STATIC);
        Table_Set(RouteFuncTable, key, funcW);
        Table_Set(RouteMimeTable,
            key, Str_CstrRef(m, "image/png"));

        key = Str_CstrRef(m, "jpg");
        Table_Set(RouteFuncTable, key, funcW);
        Table_Set(RouteMimeTable,
            key, Str_CstrRef(m, "image/jpeg"));

        key = Str_CstrRef(m, "js");
        funcW = Func_Wrapped(m, routeFuncStatic);
        funcW->type.state |= (ROUTE_SCRIPT|ROUTE_STATIC|ROUTE_ASSET);
        Table_Set(RouteFuncTable, key, funcW);
        Table_Set(RouteMimeTable,
            key, Str_CstrRef(m, "text/javascript"));

        key = Str_CstrRef(m, "css");
        funcW = Func_Wrapped(m, routeFuncStatic);
        funcW->type.state |= (ROUTE_STATIC|ROUTE_ASSET);
        Table_Set(RouteFuncTable, key, funcW);
        Table_Set(RouteMimeTable,
            key, Str_CstrRef(m, "text/css"));

        key = Str_CstrRef(m, "body");
        funcW = Func_Wrapped(m, routeFuncStatic);
        funcW->type.state |= (ROUTE_STATIC);
        Table_Set(RouteFuncTable, key, funcW);
        Table_Set(RouteMimeTable,
            key, Str_CstrRef(m, "text/html"));

        key = Str_CstrRef(m, "txt");
        funcW = Func_Wrapped(m, routeFuncStatic);
        funcW->type.state |= (ROUTE_STATIC|ROUTE_ASSET);
        Table_Set(RouteFuncTable, key, funcW);
        Table_Set(RouteMimeTable,
            key, Str_CstrRef(m, "text/plain"));

        key = Str_CstrRef(m, "fmt");
        funcW = Func_Wrapped(m, routeFuncFmt);
        funcW->type.state |= ROUTE_FMT;
        Table_Set(RouteFuncTable, key, funcW);
        Table_Set(RouteMimeTable,
            key, Str_CstrRef(m, "text/html"));

        key = Str_CstrRef(m, "d");
        funcW = Func_Wrapped(m, routeFuncFileDb);
        funcW->type.state |= ROUTE_FILEDB;
        Table_Set(RouteFuncTable, key, funcW);
        Table_Set(RouteMimeTable,
            key, Str_CstrRef(m, "application/json"));

        key = Str_CstrRef(m, "templ");
        funcW = Func_Wrapped(m, routeFuncTempl);
        funcW->type.state |= ROUTE_DYNAMIC;
        Table_Set(RouteFuncTable, key, funcW);
        Table_Set(RouteMimeTable,
            key, Str_CstrRef(m, "text/html"));

        key = Str_CstrRef(m, "config");
        funcW = Func_Wrapped(m, NULL);
        funcW->type.state |= ROUTE_FORBIDDEN;
        Table_Set(RouteFuncTable, key, funcW);
        Table_Set(RouteMimeTable,
            key, Str_CstrRef(m, "text/plain"));
    }

    return r;
}
