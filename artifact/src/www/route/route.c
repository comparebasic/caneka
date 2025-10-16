#include <external.h>
#include <caneka.h>

static status dir(MemCh *m, Str *path, Abstract *source){
    Abstract *args[3];
    args[0] = (Abstract *)path,
    args[1] = NULL,
    Out("^p. RouteDir->path @^0\n", args);
    return NOOP;
}

static status file(MemCh *m, Str *path, Str *file, Abstract *source){
    Abstract *args[5];

    Route *rt = (Route *)source;
    Str *rtPath = (Str *)as(Object_GetProp(rt, Str_CstrRef(m, "path")), TYPE_STR);
    Str *local = Str_Clone(m, path);

    StrVec *v = StrVec_From(m, file);
    IoUtil_Annotate(m, v);
    Str *base = Span_Get(v->p, 0);
    Str *ext = Span_Get(v->p, 2);

    if(!Equals((Abstract *)base, (Abstract *)Str_CstrRef(m, "index"))){
        PathStr_StrAdd(local, base);
        Str_Add(local, (byte *)"/", 1);
    }
    Str_Incr(local, rtPath->length);

    StrVec *objPath = StrVec_From(m, local);
    IoUtil_Annotate(m, objPath);

    Str *absPath = Str_Clone(m, path);
    PathStr_StrAdd(absPath, file);
    if(objPath == NULL || objPath->p->nvalues == 0 && 
            Equals(
                (Abstract *)base,
                (Abstract *)Str_CstrRef(m, "index"))){
        Route_SetTargetFile(rt, ext, absPath);
    }else{
        Route *subRt = Object_ByPath(rt, objPath, NULL, SPAN_OP_RESERVE);
        Route_SetTargetFile(subRt, ext, absPath);
    }

    args[0] = (Abstract *)objPath,
    args[1] = (Abstract *)ext,
    args[2] = (Abstract *)rt,
    args[3] = NULL,
    Out("^p. Route->file & -> @\n    &^0\n", args);
    return NOOP;
}

status Route_SetTargetFile(Route *rt, Str *ext, Str *absPath){
    Object_SetProp(rt, ext, (Abstract *)absPath);
    return SUCCESS;
}

status Route_Collect(Route *rt, StrVec *path){
    MemCh *m = Object_GetMem(rt);
    Str *public = Path_StrAdd(m, path, Str_CstrRef(m, "public"));
    Object_SetProp(rt, Str_CstrRef(m, "path"), (Abstract *)public);
    Abstract *args[2];
    args[0] = (Abstract *)public;
    args[1] = NULL;
    Out("^y.Collection & ^0\n", args);
    return Dir_Climb(m, public, dir, file, (Abstract *)rt);
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
