#include <external.h>
#include <caneka.h>

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
        route = Inst_ByPath(root, name, NULL, SPAN_OP_GET, NULL);
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

    Inst_ByPath(root, name, rt, SPAN_OP_SET, NULL);

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

    */
    /* setup Gen(s) */

    DebugStack_Pop();
    return NOOP;
}

Route *Route_Get(Route *root, StrVec *path){
    return Inst_ByPath(root, path, NULL, SPAN_OP_GET, NULL);
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
    Table_Set(seel, S(m, "gens"), I16_Wrapped(m, TYPE_SPAN));
    r |= Seel_Seel(m, seel, S(m, "Route"), TYPE_WWW_ROUTE);

    return r;
}
