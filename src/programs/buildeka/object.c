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

status BuildCtx_BuildObject(BuildCtx *ctx, StrVec *name, DirSel *sel){
    MemCh *m = ctx->m;
    /*
    Debug_Push(m, ctx->current.source);
    */
    void *args[8];
    status r = READY;
    /*

    if(ctx->type.state & DEBUG){
        args[0] = name;
        args[1] = ctx->current.source;
        args[2] = ctx->current.dest;
        args[3] = sel;
        args[4] = NULL;
        Out("^y.BuildObject name:@\n    source:@ ->\n    dest:@ sel:@^0\n", args);
    }

    if(ctx->current.binDest){
        ctx->cli.fields.current[BUILIDER_CLI_ACTION] = K(m, "Build Exec");
        ctx->cli.fields.current[BUILIDER_CLI_SOURCE] = ctx->current.source;
        ctx->cli.fields.current[BUILIDER_CLI_DEST] = ctx->current.binDest;
    }else{
        ctx->cli.fields.current[BUILIDER_CLI_ACTION] = K(m, "Build Object");
        ctx->cli.fields.current[BUILIDER_CLI_SOURCE] = ctx->current.source;
        ctx->cli.fields.current[BUILIDER_CLI_DEST] = ctx->current.dest;
    }
    BuildCtx_Log(ctx);

    Span *cmd = Span_Make(m);

    Span_Add(cmd, ctx->tools.cc);
    Span_AddSpan(cmd, ctx->input.cflags);
    Span_AddSpan(cmd, ctx->current.inc);
    Span_AddSpan(cmd, ctx->current.flags);

    if(ctx->current.binDest){
        Span_Add(cmd, Str_CstrRef(m, "-o"));
        Span_Add(cmd, StrVec_Str(m, ctx->current.binDest));
        Span_Add(cmd, StrVec_Str(m, ctx->current.source));
        if(ctx->current.target != NULL){
            Span_Add(cmd, StrVec_Str(m, ctx->current.target));
        }

        Span_AddSpan(cmd, ctx->current.staticlibs);
        Span_AddSpan(cmd, ctx->input.libs);
        if(ctx->current.liblist != NULL && ctx->current.liblist->nvalues > 0){
            Span_AddSpan(cmd, ctx->current.liblist);
        }
    }else{
        Span_Add(cmd, Str_CstrRef(m, "-g"));
        Span_Add(cmd, Str_CstrRef(m, "-c"));
        Span_Add(cmd, Str_CstrRef(m, "-o"));
        Span_Add(cmd, StrVec_Str(m, ctx->current.dest));
        Span_Add(cmd, StrVec_Str(m, ctx->current.source));
        Span_AddSpan(cmd, ctx->input.libs);
    }

    ProcDets pd;
    ProcDets_Init(m, &pd);
    r |= SubProcess(m, cmd, &pd);
    if(r & ERROR){
        void *args[] = {
            cmd,
            NULL
        };
        Fatal(ctx->m, FUNCNAME, FILENAME, LINENUMBER, "Build error for source file: @", args);
        Return(m, ERROR);
    }
    */

    Return(m, r);
}

BuildObject *BuildObject_Make(MemCh *m, BuildCtx *ctx, i32 modIdx, i32 idx){
    BuildObject *obj = MemCh_AllocOf(m, sizeof(BuildObject), TYPE_BUILD_OBJECT);
    obj->type.of = TYPE_BUILD_OBJECT;

    obj->md = Span_Get(ctx->depsOrdered, modIdx);

    if(obj->md->sel == NULL || obj->md->sel->dest == NULL){
        Error(m, FUNCNAME, FUNCNAME, LINENUMBER, 
            "Error no source files in module", NULL);
        obj->type.state |= ERROR;
        return obj;
    }

    obj->src = Span_Get(obj->md->sel->dest, idx);
    StrVec *local = Clone(m, obj->src);
    StrVec_Incr(local, ctx->src->total+1);
    obj->dest = Clone(m, ctx->dest);
    IoUtil_AddVec(m, obj->dest, local);
    
}
