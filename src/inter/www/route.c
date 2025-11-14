#include <external.h>
#include <caneka.h>

struct span *RouteFuncTable = NULL;
struct span *RouteMimeTable = NULL;

static status fileFunc(MemCh *m, Str *path, Str *file, void *source){
    void *args[5];
    RouteCtx *rctx = (RouteCtx *)source;

    StrVec *abs = IoUtil_AbsVec(m, StrVec_From(m, path));
    IoUtil_Add(m, abs, IoPath(m, "/"));
    IoUtil_Add(m, abs, StrVec_From(m, file));

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
    
    StrVec *objPath = IoPath_From(m, path);
    IoUtil_Add(m, objPath, IoPath(m, "/"));
    IoUtil_Add(m, objPath, StrVec_From(m, file));

    StrVec_Incr(objPath, rctx->path->total);

    Str *index = Str_FromCstr(m, "index", ZERO);
    word flags = ZERO;
    Route *subRt = NULL;
    if(Equals(name, index)){
        flags |= ROUTE_INDEX;
        StrVec_Decr(objPath, ext->total+1);
        StrVec_Decr(objPath, index->length);
        subRt = Inst_ByPath(rctx->root, objPath, NULL, SPAN_OP_GET);
        if(subRt == NULL){
            subRt = Inst_Make(m, TYPE_WWW_ROUTE);
            Inst_ByPath(rctx->root, objPath, subRt, SPAN_OP_SET);
        }
    }else{
        if(funcW->type.state & ROUTE_ASSET){
            Path_JoinBase(m, objPath);
        }else{
            StrVec_Decr(objPath, ext->total+1);
        }
        subRt = Inst_Make(m, TYPE_WWW_ROUTE);
        Inst_ByPath(rctx->root, objPath, subRt, SPAN_OP_SET);
    }

    Span_Set(subRt, ROUTE_PROPIDX_PATH, objPath);
    Span_Set(subRt, ROUTE_PROPIDX_FILE, abs);
    Span_Set(subRt, ROUTE_PROPIDX_FUNC, funcW);
    Span_Set(subRt, ROUTE_PROPIDX_MIME, mime);
    Span_Set(subRt, ROUTE_PROPIDX_TYPE, ext);
    subRt->type.state |= (funcW->type.state|flags);

    Route_Prepare(subRt, rctx);

    return SUCCESS;
}

static status routeFuncStatic(Buff *bf, void *action, Table *_data, void *_source){
    Str *pathS = StrVec_Str(bf->m, (StrVec *)as(action, TYPE_STRVEC));
    bf->type.state |= BUFF_UNBUFFERED;
    return File_Open(bf, pathS, O_RDONLY);
}

static status routeFuncTempl(Buff *bf, void *action, Table *data, void *source){
    Templ *templ = (Templ *)as(action, TYPE_TEMPL);

    Templ_Reset(templ);
    templ->type.state |= bf->type.state;
    status r = Templ_ToS(templ, bf, data, source);
    return r;
}

static status routeFuncFmt(Buff *bf, void *action, Table *_data, void *source){
    return Fmt_ToHtml(bf, (Mess *)as(action, TYPE_MESS));
}

static status routeFuncFileDb(Buff *bf, void *action, Table *data, void *source){
    StrVec *path = StrVec_From(bf->m, Str_FromCstr(bf->m, "filedb.keys", ZERO));
    Path_DotAnnotate(bf->m, path);
    Table *keys = (Table *)Table_ByPath(data, path, NULL, SPAN_OP_GET);

    FileDB *fdb = (FileDB *)as(action, TYPE_FILEDB);
    Table *tbl = FileDB_ToTbl(fdb, keys);

    /* tbl to json */
    return NOOP;
}

static status Route_addConfigData(RouteCtx *ctx, Route *rt, StrVec *token){
    MemCh *m = rt->m;
    void *args[3];

    StrVec *path = StrVec_Make(m);
    StrVec_AddVec(path, ctx->path);
    StrVec_AddVec(path, (StrVec *)token);

    Str *pathS = StrVec_StrCombo(m,
        path, Str_FromCstr(m, ".config", ZERO));

    NodeObj *config = Config_FromPath(m, pathS);
    if(config != NULL && config->nvalues > 0){
        Table *tbl = Seel_Get(rt, S(m, "data"));
        Table_SetByCstr(tbl, "config", config);
        return SUCCESS;
    }
    return NOOP;
}

Single *Route_MimeFunc(StrVec *path){
    StrVec *ext = Path_Ext(path->p->m, path);
    return (Single *)Table_Get(RouteFuncTable, ext);
}

Route *Route_GetNav(Route *rt){
    MemCh *m = rt->m;
    Route *nav = Route_Make(m);
    return nav;
}

status Route_Prepare(Route *rt, RouteCtx *ctx){
    DebugStack_Push(rt, rt->type.of);
    MemCh *m = rt->m;
    void *args[3];

    StrVec *path = (StrVec *)Seel_Get(rt, S(m, "file"));
    StrVec *type = (StrVec *)Seel_Get(rt, S(m, "type"));
    StrVec *token = StrVec_Make(m);
    StrVec_AddVec(token, (StrVec *)Seel_Get(rt, S(m, "path")));
    if(rt->type.state & ROUTE_INDEX){
        StrVec_Add(token, Str_FromCstr(m, "index", ZERO));
    }
    Single *funcW = (Single *)as(
        Seel_Get(rt, S(m, "func")),
        TYPE_WRAPPED_FUNC
    );

    if(funcW == NULL || (funcW->type.state & ROUTE_STATIC)){
        Span_Set(rt, ROUTE_PROPIDX_ACTION, path);
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

        Span_Set(rt, ROUTE_PROPIDX_ACTION, rbl->dest);
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

        Span_Set(rt, ROUTE_PROPIDX_ACTION, templ);
        DebugStack_Pop();
        return SUCCESS;
    }else if(funcW->type.state & ROUTE_FILEDB){
        FileDB *fdb = FileDB_Make(m, pathS);
        Span_Set(rt, ROUTE_PROPIDX_ACTION, fdb);
        DebugStack_Pop();
        return SUCCESS;
    }

    DebugStack_Pop();
    return NOOP;
}

Route *Route_GetHandler(Route *rt, StrVec *_path){
    MemCh *m = rt->m;
    StrVec *ext = Path_Ext(m, _path);
    Single *funcW = (Single *)Table_Get(RouteFuncTable, ext);

    StrVec *path = NULL;
    if(funcW != NULL){
        if(funcW->type.state & ROUTE_ASSET){
            path = Path_ReJoinExt(m, _path);
        }else{
            void *args[2];
            path = Path_WithoutExt(m, _path);
        }
    }else{
        path = _path;
    }

    Route *handler = (Route *)Inst_ByPath(rt, path, NULL, SPAN_OP_GET);

    return handler;
}

status Route_Handle(Route *rt, Buff *bf, Table *data, void *source){
    DebugStack_Push(rt, rt->type.of);
    StrVec *path = (StrVec *)Seel_Get(rt, S(bf->m, "file"));
    MemCh *m = bf->m;

    word fl = bf->m->type.state;
    bf->m->type.state |= MEMCH_BASE;
    DebugStack_SetRef(StrVec_Str(bf->m, path), TYPE_STR);
    bf->m->type.state = fl;

    Abstract *action = Seel_Get(rt, S(m, "action"));
    Single *funcW = (Single *)as(Seel_Get(rt, S(m, "func")), TYPE_WRAPPED_FUNC);

    RouteFunc func = (RouteFunc)funcW->val.ptr;
    status r = func(bf, action, data, source);

    DebugStack_Pop();
    return r;
}

status Route_Collect(Route *rt, StrVec *path){
    MemCh *m = rt->m;
    IoUtil_Annotate(m, path);

    StrVec *root = StrVec_From(m, Str_CstrRef(m, "/"));
    IoUtil_Annotate(m, root);
    Span_Set(rt, ROUTE_PROPIDX_PATH, root);

    RouteCtx ctx;
    ctx.type.of = TYPE_ROUTE_CTX;
    ctx.type.state = ZERO;
    ctx.root = rt;
    ctx.path = path;

    return Dir_Climb(m, StrVec_Str(m, path), NULL, fileFunc, &ctx);
}

Route *Route_Make(MemCh *m){
    return Inst_Make(m, TYPE_WWW_ROUTE);
}

Route *Route_From(MemCh *m, StrVec *dir){
    Route *rt = (Route *)Route_Make(m);
    Route_Collect(rt, dir);
    return rt;
}

status Route_ClsInit(MemCh *m){
    status r = READY;

    Table *seel = Table_Make(m);
    Table_Set(seel, S(m, "path"), I16_Wrapped(m, TYPE_STRVEC));
    Table_Set(seel, S(m, "data"), I16_Wrapped(m, TYPE_TABLE));
    Hashed *h = Table_SetHashed(seel, S(m, "children"), I16_Wrapped(m, TYPE_TABLE));
    /* Node End */
    Table_Set(seel, S(m, "file"), I16_Wrapped(m, TYPE_STRVEC));
    Table_Set(seel, S(m, "func"), I16_Wrapped(m, TYPE_WRAPPED_FUNC));
    Table_Set(seel, S(m, "mime"), I16_Wrapped(m, TYPE_STR));
    Table_Set(seel, S(m, "type"), I16_Wrapped(m, TYPE_STRVEC));
    Table_Set(seel, S(m, "action"), I16_Wrapped(m, TYPE_ABSTRACT));
    Table_Set(seel, S(m, "addStep"), I16_Wrapped(m, TYPE_WRAPPED_PTR));
    r |= Seel_Seel(m, seel, S(m, "Route"), TYPE_WWW_ROUTE, h->orderIdx);

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
