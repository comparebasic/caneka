#include <external.h>
#include <caneka.h>

static status dir(MemCh *m, Str *path, Abstract *source){
    return NOOP;
}

static status file(MemCh *m, Str *path, Str *file, Abstract *source){
    Route *rt = (Route *)source;

    StrVec *rtPath = (StrVec *)as(Object_GetProp(rt, Str_CstrRef(m, "path")), TYPE_STRVEC);
    Str *local = Str_CloneAlloc(m, path, STR_DEFAULT);

    StrVec *v = StrVec_From(m, file);
    IoUtil_Annotate(m, v);

    Str *base = Span_Get(v->p, 0);
    Str *ext = Span_Get(v->p, 2);

    if(!Equals((Abstract *)base, (Abstract *)Str_CstrRef(m, "index"))){
        PathStr_StrAdd(local, base);
        Str_Add(local, (byte *)"/", 1);
    }

    Str_Incr(local, rtPath->total);

    StrVec *objPath = StrVec_From(m, local);
    IoUtil_Annotate(m, objPath);

    Str *absPath = Str_CloneAlloc(m, path, STR_DEFAULT);
    PathStr_StrAdd(absPath, file);
    if(objPath == NULL || objPath->p->nvalues == 0 && 
            Equals(
                (Abstract *)base,
                (Abstract *)Str_CstrRef(m, "index"))){
        Route_SetTargetFile(rt, ext, absPath);
    }else{
        Route *subRt = Object_ByPath(rt, objPath, NULL, SPAN_OP_RESERVE);
        Object_SetPropByIdx(subRt, ROUTE_PROPIDX_PATH, (Abstract *)objPath);
        Route_SetTargetFile(subRt, ext, absPath);
    }

    return NOOP;
}

status Route_SetTargetFile(Route *rt, Str *ext, Str *absPath){
    Object_SetProp(rt, ext, (Abstract *)absPath);
    return SUCCESS;
}

status Route_Collect(Route *rt, StrVec *path){
    MemCh *m = Object_GetMem(rt);
    Object_SetProp(rt, Str_CstrRef(m, "path"), (Abstract *)path);
    Abstract *args[2];
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
    Class_SetupProp(cls, Str_CstrRef(m, "html"));
    Class_SetupProp(cls, Str_CstrRef(m, "action"));
    Class_SetupProp(cls, Str_CstrRef(m, "fmt"));
    Class_SetupProp(cls, Str_CstrRef(m, "templ"));
    r |= Class_Register(m, cls);
    return r;
}
