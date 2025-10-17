#include <external.h>
#include <caneka.h>

struct span *RouteFuncTable = NULL;
struct span *RouteMimeTable = NULL;

static status dir(MemCh *m, Str *path, Abstract *source){
    return NOOP;
}

static status file(MemCh *m, Str *path, Str *file, Abstract *source){
    Route *rt = (Route *)source;

    StrVec *pathV = StrVec_From(m, path);
    IoUtil_Annotate(m, pathV);
    StrVec *rtPath = (StrVec *)Object_GetPropByIdx(rt, ROUTE_PROPIDX_PATH);
    StrVec *objPath = Path_SubClone(m, (Abstract *)pathV, rtPath->p->max_idx+1);

    StrVec *abs = StrVec_From(m, path);
    Path_AddSlash(m, pathV);
    StrVec_Add(pathV, file);
    IoUtil_Annotate(m, abs);

    StrVec *name = Path_Name(m, pathV);
    StrVec *ext = Path_Ext(m, pathV);

    Str *mime = (Str *)Table_Get(RouteMimeTable, ext);
    Single *funcW = (Single *)Table_Get(RouteFuncTable, ext);

    if(mime == NULL){
        Abstract *args[] = {
            (Abstract *)abs,
            (Abstract *)ext,
            NULL
        };
        Error(m, (Abstract *)path, FUNCNAME, FILENAME, LINENUMBER,
            "Mime not found for this file $ with ext $", args);
        return ERROR;
    }

    Str *index = Str_CstrRef(m, "index");
    if(!Equals((Abstract *)nam, (Abstract *)index)){
        Path_Add(objPath, name);
    }

    if(objPath == NULL || objPath->p->nvalues == 0 && 
            Equals(
                (Abstract *)base,
                (Abstract *)Str_CstrRef(m, "index"))){
        Object_SetPropByIdx(rt, ROUTE_PROPIDX_FILE, (Abstract *)abs);
        Object_SetPropByIdx(rt, ROUTE_PROPIDX_TYPE, (Abstract *)ext);
        Object_SetPropByIdx(rt, ROUTE_PROPIDX_FUNC, (Abstract *)funcW);
        Object_SetPropByIdx(rt, ROUTE_PROPIDX_MIME, (Abstract *)mime);
        rt->type.state |= funcW->type.state;
    }else{
        Route *subRt = Object_ByPath(rt, objPath, NULL, SPAN_OP_RESERVE);
        subRt->type.state |= funcW->type.state;
        Object_SetPropByIdx(subRt, ROUTE_PROPIDX_PATH, (Abstract *)objPath);
        Object_SetPropByIdx(subRt, ROUTE_PROPIDX_FILE, (Abstract *)abs);
        Object_SetPropByIdx(subRt, ROUTE_PROPIDX_FUNC, (Abstract *)funcW);
        Object_SetPropByIdx(subRt, ROUTE_PROPIDX_MIME, (Abstract *)mime);
    }

    return NOOP;
}

static status routeFuncStatic(StrVec *in, StrVec *out, Object *_data, Abstract *_source){
    StrVec_AddVec(out, in);
    return SUCCESS;
}

static status routeFuncTempl(MemCh *m,
        StrVec *in, StrVec *out, Object *data, Abstract *source){

    Cursor *curs = Cursor_Make(m, in);
    TemplCtx *ctx = TemplCtx_FromCurs(m, curs, source);
    Templ *templ = (Templ *)Templ_Make(m, ctx->it.p);
    Templ_Prepare(templ);

    Stream *sm = Stream_MakeToVec(m, out);
    Templ_ToS(templ, sm, (Abstract *)data, source);

    return templ->type.state;
}

static status routeFuncFmt(StrVec *in, StrVec *out, Object *_data, Abstract *_source){
    Cursor *curs = Cursor_Make(m, in);
    Roebling * = FormatFmt_Make(m, curs, NULL);
    Roebling_Run(rbl);
        
    Stream *sm = Stream_MakeToVec(m, out);
    Fmt_ToHtml(sm, rbl->mess);

    return rbl->type.state;
}

status Route_Handle(MemCh *m, Route *rt, StrVec *dest, Object *data, Abstract *source){
    StrVec *path = (StrVec *)Object_GetPropByIdx(rt, ROUTE_PROPIDX_FILE);
    StrVec *type = (StrVec *)Object_GetPropByIdx(rt, ROUTE_PROPIDX_TYPE);
    Single *funcW = (Single *)as(
        Object_GetPropByIdx(rt, ROUTE_PROPIDX_FUNC),
        TYPE_WRAPPED_FUNC
    );

    if(funcW != NULL){
        StrVec *v = StrVec_Make(m);
        FileDes_ToVec(v, path);
        RouteFunc func = (RouteFunc)funcW->val.ptr;
        return func(v, dest, data, source);
    }else{
        FileDes_ToVec(dest, path);
        return dest->type.state;
    }
}

status Route_Collect(Route *rt, StrVec *path){
    MemCh *m = Object_GetMem(rt);

    StrVec *pathV = StrVec_From(m, path);
    IoUtil_Annotate(m, pathV);

    Object_SetPropByIdx(rt, ROUTE_PROPIDX_PATH, (Abstract *)pathV);
    return Dir_Climb(m, StrVec_Str(m, path), dir, file, (Abstract *)rt);
}

Nav *Route_Make(MemCh *m){
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

        key = Str_CstrRef(m, "txt");
        funcW = Func_Wrapped(m, routeFuncStatic);
        funcW->type.state |= ROUTE_STATIC;
        Table_Set(RouteFuncTable, (Abstract *)key, (Abstract *)funcW);
        Table_Set(RouteMimeTable,
            (Abstract *)key, (Abstract *)Str_CstrRef(m, "text/plain"));

        key = Str_CstrRef(m, "fmt");
        funcW = Func_Wrapped(m, routeFuncFmt);
        funcW->type.state |= ROUTE_DYNAMIC;
        Table_Set(RouteFuncTable, (Abstract *)key, (Abstract *)funcW);
        Table_Set(RouteMimeTable,
            (Abstract *)key, (Abstract *)Str_CstrRef(m, "text/html"));

        key = Str_CstrRef(m, "templ");
        funcW = Func_Wrapped(m, routeFuncTempl);
        funcW->type.state |= ROUTE_DYNAMIC;
        Table_Set(RouteFuncTable, (Abstract *)key, (Abstract *)funcW);
        Table_Set(RouteMimeTable,
            (Abstract *)key, (Abstract *)Str_CstrRef(m, "text/html"));
    }

    return r;
}
