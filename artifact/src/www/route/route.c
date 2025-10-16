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
    path = Str_Clone(m, path);

    StrVec *v = StrVec_From(m, file);
    IoUtil_Annotate(m, v);
    Str *base = Span_Get(v->p, 0);
    Str *ext = Span_Get(v->p, 2);

    Str_Add(path, (byte *)"/", 1);
    Str_Add(path, base->bytes, base->length);
    Str_Incr(path, rtPath->length);

    StrVec *objPath = StrVec_From(m, path);
    IoUtil_Annotate(m, objPath);

    args[0] = (Abstract *)objPath,
    args[1] = (Abstract *)ext,
    args[2] = NULL,
    Out("^p. Route->file & -> @^0\n", args);
    return NOOP;
}

status Route_Collect(Route *rt, StrVec *path){
    MemCh *m = Object_GetMem(rt);
    Str *public = Path_StrAdd(m, path, Str_CstrRef(m, "public/"));
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
