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

status Route_Handle(Route *rt, Stream *sm, Object *data, Abstract *source){
    Str *action = (Str *)Object_GetPropByIdx(rt, ROUTE_PROPIDX_ACTION);
    Str *html = (Str *)Object_GetPropByIdx(rt, ROUTE_PROPIDX_HTML);
    Str *templ = (Str *)Object_GetPropByIdx(rt, ROUTE_PROPIDX_TEAMPL);
    Str *fmt = (Str *)Object_GetPropByIdx(rt, ROUTE_PROPIDX_FMT);
    if(action != NULL){
        File *f = File_Make(m, action, NULL, STREAM_STRVEC);
        File_Open(f);
        File_Read(f, FILE_READ_MAX);
        File_Close(f);

        Cursor *curs = File_GetCurs(f);
        TemplCtx *ctx = TemplCtx_FromCurs(m, curs, NULL);

        Templ *templ = (Templ *)Templ_Make(m, ctx->it.p);
        status result = Templ_Prepare(templ);
        Templ_ToS(templ, sm, (Abstract *)data, source);
        return SUCCESS;
    }else if(templ != NULL &&
            (html == NULL || IoUtil_CmpUpdated(sm->m, html, templ, NULL))){
        File *f = File_Make(m, templ, NULL, STREAM_STRVEC);
        File_Open(f);
        File_Read(f, FILE_READ_MAX);
        File_Close(f);

        Cursor *curs = File_GetCurs(f);
        TemplCtx *ctx = TemplCtx_FromCurs(m, curs, NULL);
        Templ *templ = (Templ *)Templ_Make(m, ctx->it.p);
        status result = Templ_Prepare(templ);
        return Templ_ToS(templ, sm, (Abstract *)data, source);
        
    }else if(fmt != NULL &&
            (html == NULL || IoUtil_CmpUpdated(sm->m, html, fmt, NULL))){
        File *f = File_Make(m, fmt, NULL, STREAM_STRVEC);
        File_Open(f);
        File_Read(f, FILE_READ_MAX);

        Cursor *curs = File_GetCurs(f);

        Roebling *rbl = NULL;
        rbl = FormatFmt_Make(m, curs, NULL);
        Roebling_Run(rbl);
        
        Fmt_ToHtml(sm, rbl->mess);
    }else if(html != NULL){
        File *f = File_Make(sm->m, html, NULL, STREAM_FROM_FD); 
        File_Open(f);
        if(f->type.state & PROCESSING){
            File_Read(f, FILE_READ_MAX);
            Stream_VecTo(sm, f->sm->dest.curs->v);
            return SUCCESS;
        }
    }
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
