#include <external.h>
#include <caneka.h>

static Span *AllExtSpan = NULL;
Span *RouteFuncTable = NULL;
Span *RouteMimeTable = NULL;

static status routeFuncStatic(Buff *bf,
        Route *rt, Table *_data, HttpCtx *ctx){
    MemCh *m = bf->m;
    Str *pathS = StrVec_Str(bf->m,
        (StrVec *)as(Seel_Get(rt, K(m, "action")), TYPE_STRVEC));
    bf->type.state |= BUFF_UNBUFFERED;
    return File_Open(bf, pathS, O_RDONLY);
}

static status routeFuncTempl(Buff *bf,
        Route *rt, Table *data, HttpCtx *ctx){
    MemCh *m = bf->m;
    Templ *templ = (Templ *)as(Seel_Get(rt, K(m, "templ")), TYPE_TEMPL);

    Templ_Reset(templ);
    templ->type.state |= bf->type.state;
    Templ_ToS(templ, bf, data, NULL);
    return templ->type.state;
}

static status routeFuncFmt(Buff *bf,
        Route *rt, Table *_data, HttpCtx *ctx){
    MemCh *m = bf->m;
    return Fmt_ToHtml(bf,
        (Mess *)as(Seel_Get(rt, K(m, "action")), TYPE_MESS));
}

static status routeFuncFileDb(Buff *bf,
        Route *rt, Table *data, HttpCtx *ctx){
    MemCh *m = bf->m;
    BinSegCtx *bsCtx = (BinSegCtx *)as(Seel_Get(rt, K(m, "action")), TYPE_BINSEG_CTX);
    Abstract *action = Table_Get(ctx->queryIt.p, K(m, "action"));
    status r = READY;
    if(action == NULL){
        ctx->code = 403;
        ctx->type.state |= ERROR;
        return ctx->type.state;
    }
    if(Equals(action, K(m, "add")) && ctx->body != NULL &&
            ctx->body->type.of == TYPE_TABLE || 
            ctx->body->type.of == TYPE_SPAN || 
            (ctx->body->type.of & TYPE_INSTANCE) 
        ){
        BinSegCtx_Send(bsCtx, ctx->body);
    }else if(Equals(action, K(m, "modify"))){
        /* modify and existing binseg record */;
    }else if(Equals(action, K(m, "read"))){
        /* read from binseg */;
    }else{
        ctx->code = 403;
        ctx->type.state |= ERROR;
        return ctx->type.state;
    }

    StrVec *acceptHeader = Table_Get(ctx->headersIt.p, K(m, "Accept"));
    if(acceptHeader != NULL && Equals(acceptHeader, K(m, "text/html"))){
        Table_Set(data, K(m, "form"), ctx->body);
        return routeFuncTempl(bf, rt, data, ctx);
    }else{
        /* handle json response here */;
    }

    return NOOP;
}

static NodeObj *Route_addConfigData(Route *rt, StrVec *token){
    MemCh *m = rt->m;

    /*
    StrVec *path = StrVec_Make(m);
    StrVec_AddVec(path, ctx->path);
    StrVec_AddVec(path, (StrVec *)token);

    Str *pathS = StrVec_StrCombo(m,
        path, Str_FromCstr(m, ".config", ZERO));

    NodeObj *config = Config_FromPath(m, pathS);
    if(config != NULL && config->nvalues > 0){
        Table *tbl = Seel_Get(rt, S(m, "data"));
        Table_SetByCstr(tbl, "config", config);
        return config;
    }
    */
    return NULL;
}

static Templ *prepareTempl(Route *rt, StrVec *path){
    void *args[3];
    MemCh *m = rt->m;
    StrVec *content = File_ToVec(m, StrVec_Str(m, path));

    if(content == NULL || content->total == 0){
        args[0] = path;
        args[1] = content;
        args[2] = NULL;
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "Error path for Templ has no content for: $ -> @", args);
        rt->type.state |= ERROR;
        return NULL;
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
        return NULL;
    }
    return templ;
}

Single *Route_MimeFunc(StrVec *path){
    StrVec *ext = Path_Ext(path->p->m, path);
    return (Single *)Table_Get(RouteFuncTable, ext);
}

Route *Route_BuildRoute(Route *root, StrVec *name, StrVec *path, Table *configAtts){
    MemCh *m = root->m;

    Route *route = NULL;

    StrVec *ext = IoUtil_GetExt(m, path);
    Str *mime = (Str *)Table_Get(RouteMimeTable, ext);
    Single *funcW = (Single *)Table_Get(RouteFuncTable, ext);

    if(mime == NULL || funcW == NULL){
        return NULL;
    }

    if(funcW->type.state & ROUTE_FORBIDDEN){
        return NULL;
    }
    
    Route *rt = Inst_Make(m, TYPE_WWW_ROUTE);

    Str *index = Str_FromCstr(m, "index", ZERO);
    word flags = ZERO;
    if(Equals(name, index)){
        flags |= ROUTE_INDEX;
        route = Inst_ByPath(root, name, NULL, SPAN_OP_GET);
        if(route == NULL){
            route = Inst_Make(m, TYPE_WWW_ROUTE);
        }
    }else{
        if((funcW->type.state & ROUTE_ASSET) == 0){
            IoUtil_SwapExt(m, name, NULL);
        }else{
            name = Path_ReJoinExt(m, name);
        }
        route = Inst_Make(m, TYPE_WWW_ROUTE);
    }

    Inst_ByPath(root, name, rt, SPAN_OP_SET);

    void *args[] = {name, path, configAtts, mime, ext, NULL};
    Out("^y.  Building path ^0.@^y.\n  @ from @\nmime @ ext @\n^0", args);

    /*
    Span_Set(subRt, ROUTE_PROPIDX_PATH, objPath);
    Span_Set(subRt, ROUTE_PROPIDX_FILE, abs);
    Span_Set(subRt, ROUTE_PROPIDX_FUNC, funcW);
    Span_Set(subRt, ROUTE_PROPIDX_MIME, mime);
    Span_Set(subRt, ROUTE_PROPIDX_TYPE, ext);
    subRt->type.state |= (funcW->type.state|flags);
    */

    return NULL;
}

status Route_CollectConfig(Route *root, StrVec *name, StrVec *path, Table *configAtts){
    MemCh *m = root->m;
    Route *rt = Route_Make(m);

    Span *ext = Config_Sequence(m, Table_Get(configAtts, K(m, "ext")));
    if(ext != NULL){
        ext = AllExtSpan;
    }

    Span *p = Span_Make(m);
    Dir_GatherByExt(m, StrVec_Str(m, path), p, ext);

    Iter it;
    Iter_Init(&it, p);
    while((Iter_Next(&it) & END) == 0){
        StrVec *fpath = Iter_Get(&it);

        name = StrVec_Copy(m, fpath);
        StrVec_Incr(name, path->total);
        IoUtil_Annotate(m, name);
        IoUtil_Annotate(m, fpath);

        Route *rt = Route_BuildRoute(root, name, fpath, configAtts);
        /*
        Route_Prepare(rt);
        */
    }

    void *args[] = {name, root, NULL};
    Out("^p.Built @ -> @^0\n", args);

    /* build route */

    return ZERO;
}

status Route_CheckEtag(Route *rt, StrVec *etag){
    MemCh *m = rt->m;
    Single *funcW = Seel_Get(rt, K(m, "func"));
    if((funcW->type.state & ROUTE_ASSET) == 0){
        return NOOP;
    }
    Table *headers = Seel_Get(rt, K(m, "headers"));
    StrVec *etagRegistered = Table_Get(headers, K(m, "Etag"));
    return (etagRegistered != NULL && Equals(etag, etagRegistered)) ? SUCCESS : MORE;
}

status Route_SetEtag(Route *rt, Str *path, struct timespec *mod){
    MemCh *m = rt->m;

    StrVec *etag = HttpCtx_MakeEtag(rt->m, path, mod);

    StrVec *etagPath = StrVec_From(m, Str_Clone(m, path));
    IoUtil_Annotate(m, etagPath);
    IoUtil_AddExt(m, etagPath, S(m, "etag"));

    Str *etagPathS = StrVec_Str(m, etagPath);
    Buff *bf = Buff_Make(m, NOOP);
    if(File_Open(bf, etagPathS, O_RDONLY) & ERROR){
        bf->type.state = BUFF_UNBUFFERED;
        File_Open(bf, etagPathS, O_CREAT|O_WRONLY);
        Buff_AddVec(bf, etag);
        File_Close(bf);
    }else{
       Buff_Read(bf); 
       File_Close(bf);
       if(!Equals(bf->v, etag)){
            bf->type.state = BUFF_UNBUFFERED|BUFF_CLOBBER;
            File_Open(bf, etagPathS, O_WRONLY|O_TRUNC);
            Buff_AddVec(bf, etag);
            File_Close(bf);
       }
    }
    Table *headers = Seel_Get(rt, K(m, "headers"));
    Table_Set(headers, S(m, "Etag"), etag);

    return ZERO;
}

status Route_Prepare(Route *rt){
    DebugStack_Push(rt, rt->type.of);
    MemCh *m = rt->m;
    void *args[3];

    /*
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

    Str *pathS = StrVec_Str(m, path);
    Table *headers = Seel_Get(rt, K(m, "headers"));
    struct timespec mod;
    File_ModTime(m, pathS, &mod);
    Table_Set(headers, K(m, "Last-Modified"), Time_ToRStr(m, &mod));

    if(funcW != NULL && (funcW->type.state & ROUTE_ASSET)){

        Route_SetEtag(rt, pathS, &mod);
    }

    if(funcW == NULL || (funcW->type.state & ROUTE_STATIC)){
        Span_Set(rt, ROUTE_PROPIDX_ACTION, path);
    }

    NodeObj *config = Route_addConfigData(ctx, rt, token);

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
        Templ *templ = prepareTempl(rt, path);
        if(templ != NULL){
            Span_Set(rt, ROUTE_PROPIDX_TEMPL, templ);
        }
        DebugStack_Pop();
        return SUCCESS;
    }else if(funcW->type.state & ROUTE_BINSEG){
        word flags = BINSEG_REVERSED;
        
        Table *seel = NULL;
        if(config != NULL){
            NodeObj *rbsConfig = Inst_ByPath(config, Sv(m, "binseg"), NULL, SPAN_OP_GET);
            StrVec *actionV = NodeObj_Att(rbsConfig, K(m, "action"));
            Span *actions = Config_Sequence(m, actionV);
            Iter it;
            Iter_Init(&it, actions);
            while((Iter_Next(&it) & END) == 0){
                Abstract *a = Iter_Get(&it);
                flags |= BinSeg_ActionByStr(a);
            }

            StrVec *seelName = NodeObj_Att(rbsConfig, K(m, "seel"));
            if(seelName != NULL){
                seel = Table_Get(SeelByName, seelName);
            }

            StrVec *incPath = StrVec_Copy(m, path);
            IoUtil_SwapExt(m, incPath, S(m, "tinc"));

            if(File_PathExists(m, StrVec_Str(m, incPath))){
                Templ *templ = prepareTempl(rt, incPath);
                if(templ != NULL){
                    Span_Set(rt, ROUTE_PROPIDX_TEMPL, templ);
                }
            }
        }

        BinSegCtx *ctx = BinSegCtx_Make(m, flags);
        BinSegCtx_Open(ctx, StrVec_Str(m, path));
        ctx->seel = seel;

        Span_Set(rt, ROUTE_PROPIDX_ACTION, ctx);
        DebugStack_Pop();
        return SUCCESS;
    }
    */

    DebugStack_Pop();
    return NOOP;
}

Route *Route_Get(Route *root, StrVec *path){
    return Inst_ByPath(root, path, NULL, SPAN_OP_GET);
}

status Route_Handle(Route *rt, Span *dest, Table *data, HttpCtx *ctx){
    DebugStack_Push(rt, rt->type.of);
    status r = READY;
    /*
    StrVec *path = (StrVec *)Seel_Get(rt, S(bf->m, "file"));
    MemCh *m = bf->m;

    word fl = bf->m->type.state;
    bf->m->type.state |= MEMCH_BASE;
    DebugStack_SetRef(StrVec_Str(bf->m, path), TYPE_STR);
    bf->m->type.state = fl;

    Abstract *action = Seel_Get(rt, S(m, "action"));
    Single *funcW = (Single *)as(Seel_Get(rt, S(m, "func")), TYPE_WRAPPED_FUNC);

    Table *headers = Seel_Get(rt, K(bf->m, "headers"));
    if(ctx != NULL && headers != NULL){
        Iter it;
        Iter_Init(&it, headers);
        while((Iter_Next(&it) & END) == 0){
            Hashed *h = Iter_Get(&it);
            if(h != NULL){
                Table_Set(ctx->headersOut, h->key, h->value);
            }
        }
    }

    RouteFunc func = (RouteFunc)funcW->val.ptr;
    status r = func(bf, rt, data, ctx);
    */

    DebugStack_Pop();
    return r;
}

Route *Route_Make(MemCh *m){
    return Inst_Make(m, TYPE_WWW_ROUTE);
}

status Route_Init(MemCh *m){
    status r = READY;

    Table *seel = Table_Make(m);
    Table_Set(seel, S(m, "path"), I16_Wrapped(m, TYPE_STRVEC));
    Table_Set(seel, S(m, "data"), I16_Wrapped(m, TYPE_TABLE));
    Hashed *h = Table_SetHashed(seel, S(m, "children"), I16_Wrapped(m, TYPE_TABLE));
    /* Node End */
    Table_Set(seel, S(m, "gens"), I16_Wrapped(m, TYPE_SPAN));
    r |= Seel_Seel(m, seel, S(m, "Route"), TYPE_WWW_ROUTE, h->orderIdx);

    if(RouteFuncTable == NULL){
        RouteFuncTable = Table_Make(m);
        RouteMimeTable = Table_Make(m);
        AllExtSpan = Span_Make(m);

        Str *key = S(m, "html");
        Span_Add(AllExtSpan, key);
        Single *funcW = Func_Wrapped(m, routeFuncStatic);
        funcW->type.state |= (ROUTE_STATIC);
        Table_Set(RouteFuncTable, key, funcW);
        Table_Set(RouteMimeTable,
            key, S(m, "text/html"));

        key = S(m, "png");
        Span_Add(AllExtSpan, key);
        funcW = Func_Wrapped(m, routeFuncStatic);
        funcW->type.state |= (ROUTE_ASSET|ROUTE_STATIC);
        Table_Set(RouteFuncTable, key, funcW);
        Table_Set(RouteMimeTable,
            key, S(m, "image/png"));

        key = S(m, "jpg");
        Span_Add(AllExtSpan, key);
        Table_Set(RouteFuncTable, key, funcW);
        Table_Set(RouteMimeTable,
            key, S(m, "image/jpeg"));

        key = S(m, "js");
        Span_Add(AllExtSpan, key);
        funcW = Func_Wrapped(m, routeFuncStatic);
        funcW->type.state |= (ROUTE_STATIC|ROUTE_ASSET);
        Table_Set(RouteFuncTable, key, funcW);
        Table_Set(RouteMimeTable,
            key, S(m, "text/javascript"));

        key = S(m, "css");
        Span_Add(AllExtSpan, key);
        funcW = Func_Wrapped(m, routeFuncStatic);
        funcW->type.state |= (ROUTE_STATIC|ROUTE_ASSET);
        Table_Set(RouteFuncTable, key, funcW);
        Table_Set(RouteMimeTable,
            key, S(m, "text/css"));

        key = S(m, "body");
        Span_Add(AllExtSpan, key);
        funcW = Func_Wrapped(m, routeFuncStatic);
        funcW->type.state |= (ROUTE_STATIC);
        Table_Set(RouteFuncTable, key, funcW);
        Table_Set(RouteMimeTable,
            key, S(m, "text/html"));

        key = S(m, "txt");
        Span_Add(AllExtSpan, key);
        funcW = Func_Wrapped(m, routeFuncStatic);
        funcW->type.state |= (ROUTE_STATIC|ROUTE_ASSET);
        Table_Set(RouteFuncTable, key, funcW);
        Table_Set(RouteMimeTable,
            key, S(m, "text/plain"));

        key = S(m, "fmt");
        Span_Add(AllExtSpan, key);
        funcW = Func_Wrapped(m, routeFuncFmt);
        funcW->type.state |= ROUTE_FMT;
        Table_Set(RouteFuncTable, key, funcW);
        Table_Set(RouteMimeTable,
            key, S(m, "text/html"));

        key = S(m, "rbs");
        Span_Add(AllExtSpan, key);
        funcW = Func_Wrapped(m, routeFuncFileDb);
        funcW->type.state |= ROUTE_BINSEG;
        Table_Set(RouteFuncTable, key, funcW);
        Table_Set(RouteMimeTable,
            key, S(m, "*/*"));

        key = S(m, "templ");
        Span_Add(AllExtSpan, key);
        funcW = Func_Wrapped(m, routeFuncTempl);
        funcW->type.state |= ROUTE_DYNAMIC;
        Table_Set(RouteFuncTable, key, funcW);
        Table_Set(RouteMimeTable,
            key, S(m, "text/html"));
    }

    return r;
}
