#include <external.h>
#include "buildeka_module.h"

StrVec *BuildObject_GetDest(MemCh *m, BuildCtx *ctx, BuildModule *md, StrVec *path){
    StrVec *local = Clone(m, path);
    StrVec_Incr(local, ctx->src->total+1);
    StrVec *dest = IoUtil_BasePath(m, md->target);
    IoUtil_AddVec(m, dest, Sv(m, "object"));

    StrVec *out = Clone(m, local);
    StrVec_Incr(out, md->local->total+1);
    IoUtil_SwapExt(m, out, S(m, "o"));
    IoUtil_AddVec(m, dest, out);
    return dest;
}

void BuildObject_Link(MemCh *m, BuildCtx *ctx, BuildObject *obj){
    status r = READY;
    Span *cmd = Span_Make(m);
    Span_Add(cmd, ctx->tools.ar);
    Span_Add(cmd, S(m, "-rcs"));
    Span_Add(cmd, Ifc(m, obj->md->target, TYPE_STR));
    Span_Add(cmd, Ifc(m, obj->dest, TYPE_STR));

    ProcDets pd;
    ProcDets_Init(m, &pd);
    r |= SubProcess(m, cmd, &pd);
    if(r & ERROR){
        void *args[] = {
            cmd,
            NULL
        };
        Fatal(ctx->m, FUNCNAME, FILENAME, LINENUMBER, "Archive error for source file: @", args);
    }
    ReturnVoid(m);
}

void BuildObject_Build(MemCh *m, BuildCtx *ctx, BuildObject *obj){
    Debug_Push(m, obj);
    void *args[8];
    status r = READY;

    ctx->log(m, obj, ctx);

    Span *cmd = Span_Make(m);

    Str *destS = Ifc(m, obj->dest, TYPE_STR);
    if(File_PathExists(m, destS)){
        File_Unlink(m, destS);
    }

    Span_Add(cmd, ctx->tools.cc);
    Span_AddSpan(cmd, ctx->current.flags);
    Span_AddSpan(cmd, obj->md->flags);
    Span_Add(cmd, S(m, "-c"));
    Span_Add(cmd, S(m, "-o"));
    Span_Add(cmd, destS);
    Span_Add(cmd, Ifc(m, obj->src, TYPE_STR));

    void *ar[] = {
        obj,
        cmd,
        NULL,
    };
    Out("^c.Building @ ->\n    @^0\n", ar);

    ProcDets pd;
    ProcDets_Init(m, &pd);

    Dir_CheckCreateFor(m, obj->dest);
    r |= SubProcess(m, cmd, &pd);
    if(r & ERROR){
        void *args[] = {
            cmd,
            NULL
        };
        Fatal(ctx->m, FUNCNAME, FILENAME, LINENUMBER, "Build error for source file: @", args);
        ReturnVoid(m);
    }

    ReturnVoid(m);
}

BuildObject *BuildObject_Current(MemCh *m, BuildCtx *ctx){
    i32 modIdx = ctx->current.moduleIt.idx;
    i32 idx = ctx->current.sourcesIt.idx;

    BuildObject *obj = MemCh_AllocOf(m, 
        sizeof(BuildObject), TYPE_BUILD_OBJECT);
    obj->type.of = TYPE_BUILD_OBJECT;

    Debug_Push(m, obj);

    obj->md = (BuildModule *)Iter_Get(&ctx->current.moduleIt);
    if(obj->md == NULL){
        Error(m, FUNCNAME, FUNCNAME, LINENUMBER, 
            "Error no module found as current in Iter", NULL);
        obj->type.state |= ERROR;
        return obj;
    }

    if(obj->md->sel == NULL || obj->md->sel->dest == NULL){
        Error(m, FUNCNAME, FUNCNAME, LINENUMBER, 
            "Error no source files in module", NULL);
        obj->type.state |= ERROR;
        return obj;
    }

    obj->src = IoUtil_Annotate(m, Iter_Get(&ctx->current.sourcesIt));
    obj->dest = BuildObject_GetDest(m, ctx, obj->md, obj->src);

    struct stat sourceSt;
    struct stat st;
    File_Stat(m, Ifc(m, obj->src, TYPE_STR), &sourceSt);
    if((File_Stat(m, Ifc(m, obj->dest, TYPE_STR), &st) & SUCCESS)
            && (sourceSt.st_mtime > st.st_mtime)){
        obj->type.state |= BUILDOBJ_SATISFIED;
    }
    
    return obj;
}
