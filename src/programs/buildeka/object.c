#include <external.h>
#include "buildeka_module.h"

StrVec *BuildCtx_DestFromSrc(BuildCtx *ctx,
        StrVec *path, StrVec *src, StrVec *dest){
    return NULL;
}

status BuildCtx_LinkObject(BuildCtx *ctx, StrVec *name, DirSel *sel){
    MemCh *m = ctx->m;
    Debug_Push(m, ctx->dest);
    /*

    ctx->cli.fields.current[BUILIDER_CLI_ACTION] = K(m, "Link Object");
    ctx->cli.fields.current[BUILIDER_CLI_SOURCE] = ctx->current.source;
    ctx->cli.fields.current[BUILIDER_CLI_DEST] = ctx->current.dest;
    BuildCtx_Log(ctx);

    ProcDets pd;
    ProcDets_Init(m, &pd);

    Span *cmd = Span_Make(m);
    Span_Add(cmd, ctx->tools.ar);
    Span_Add(cmd, Str_CstrRef(m, "-rc"));
    Span_Add(cmd, StrVec_Str(m, ctx->current.target));
    Span_Add(cmd, StrVec_Str(m, ctx->current.dest));

    ProcDets_Init(m, &pd);
    status re = SubProcess(m, cmd, &pd);
    if(re & ERROR){
        Debug_SetRef(m, cmd);
        Fatal(ctx->m, FUNCNAME, FILENAME, LINENUMBER, 
            "Build error for adding object to lib", NULL);
        Return(m, ERROR);
    }
    */

    Return(m, ZERO);
}

void BuildObject_Build(MemCh *m, BuildCtx *ctx, BuildObject *obj){
    Debug_Push(m, obj);
    void *args[8];
    status r = READY;

    ctx->log(m, obj, ctx);

    Span *cmd = Span_Make(m);

    Span_Add(cmd, ctx->tools.cc);
    Span_AddSpan(cmd, ctx->current.flags);
    Span_AddSpan(cmd, obj->md->flags);
    Span_Add(cmd, S(m, "-c"));
    Span_Add(cmd, S(m, "-o"));
    Span_Add(cmd, Ifc(m, obj->dest, TYPE_STR));
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

    Hashed *h = Iter_Get(&ctx->current.moduleIt);
    if(h == NULL || h->value == NULL){
        Error(m, FUNCNAME, FUNCNAME, LINENUMBER, 
            "Error no module found as current in Iter", NULL);
        obj->type.state |= ERROR;
        return obj;
    }

    obj->md = (BuildModule *)h->value;
    if(obj->md->sel == NULL || obj->md->sel->dest == NULL){
        Error(m, FUNCNAME, FUNCNAME, LINENUMBER, 
            "Error no source files in module", NULL);
        obj->type.state |= ERROR;
        return obj;
    }

    obj->src = IoUtil_Annotate(m, Iter_Get(&ctx->current.sourcesIt));
    StrVec *local = Clone(m, obj->src);
    StrVec_Incr(local, ctx->src->total+1);
    obj->dest = IoUtil_BasePath(m, obj->md->target);
    IoUtil_AddVec(m, obj->dest, Sv(m, "object"));
    StrVec *out = Clone(m, local);

    StrVec_Incr(out, obj->md->local->total+1);
    IoUtil_SwapExt(m, out, S(m, "o"));
    IoUtil_AddVec(m, obj->dest, out);

    struct stat sourceSt;
    struct stat st;
    File_Stat(m, Ifc(m, obj->src, TYPE_STR), &sourceSt);
    if((File_Stat(m, Ifc(m, obj->dest, TYPE_STR), &st) & SUCCESS)
            && (sourceSt.st_mtime > st.st_mtime)){
        obj->type.state |= BUILDOBJ_SATISFIED;
    }
    
    return obj;
}
