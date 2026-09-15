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
    Span_Add(cmd, S(m, "-rc"));
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
    if((obj->type.state & BUILDOBJ_EXEC) == 0){
        Span_Add(cmd, S(m, "-c"));
    }
    Span_Add(cmd, S(m, "-o"));
    Span_Add(cmd, destS);
    Span_Add(cmd, Ifc(m, obj->src, TYPE_STR));

    if(obj->type.state & BUILDOBJ_EXEC){
        Span_AddSpan(cmd, ctx->current.statLibs);
        Span_AddSpan(cmd, ctx->current.libPaths);
        Span_AddSpan(cmd, ctx->current.libs);
    }

    ctx->log(m, ctx, obj);
    if(obj->type.state & BUILDOBJ_EXEC){
        void *ar[] = {
            obj,
            NULL
        };
        Out("^p.    Obj: @\n^0", ar);
        Iter it;
        Iter_Init(&it, cmd);
        while((Iter_Next(&it) & END) == 0){
            ToS(OutStream, Iter_Get(&it), ZERO, ZERO); 
            if(it.type.state & LAST){
                Buff_AddBytes(OutStream, (byte *)"\n", 1);
            }else{
                Buff_AddBytes(OutStream, (byte *)" ", 1);
            }
        }
    }

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

BuildObject *BuildObject_Inc(MemCh *m, BuildCtx *ctx, BuildModule *md){
    BuildObject *obj = MemCh_AllocOf(m, 
        sizeof(BuildObject), TYPE_BUILD_OBJECT);
    obj->type.of = TYPE_BUILD_OBJECT;
    Debug_Push(m, obj);
    obj->md = md;

    obj->src = Clone(m, md->src);
    IoUtil_AddVec(m, obj->src, Sv(m, "inc.c"));
    obj->dest = md->target;

    Return(m, obj);
}


BuildObject *BuildObject_Exec(MemCh *m, BuildCtx *ctx, BuildModule *md, StrVec *path){
    BuildObject *obj = MemCh_AllocOf(m, 
        sizeof(BuildObject), TYPE_BUILD_OBJECT);
    obj->type.of = TYPE_BUILD_OBJECT;
    obj->type.state |= BUILDOBJ_EXEC;
    Debug_Push(m, obj);

    obj->md = md;

    obj->src = IoUtil_Annotate(m, path);
    obj->dest = Clone(m, ctx->dest);
    IoUtil_AddVec(m, obj->dest, Sv(m, "bin"));

    StrVec *local = Clone(m, path);
    StrVec_Incr(local, ctx->src->total+1);
    StrVec_Incr(local, md->local->total+1);
    IoUtil_SwapExt(m, local, NULL);
    if(Equals(local, K(m, "main"))){
        local = md->name;
    }
    IoUtil_AddVec(m, obj->dest, local);
    Return(m, obj);
}

BuildObject *BuildObject_From(MemCh *m,
        BuildCtx *ctx, BuildModule *md, StrVec *path){
    BuildObject *obj = MemCh_AllocOf(m, 
        sizeof(BuildObject), TYPE_BUILD_OBJECT);
    obj->type.of = TYPE_BUILD_OBJECT;

    Debug_Push(m, obj);

    obj->md = md;
    if(obj->md == NULL){
        Error(m, FUNCNAME, FUNCNAME, LINENUMBER, 
            "Error no module found as current in Iter", NULL);
        obj->type.state |= ERROR;
        Return(m, obj);
    }

    if(obj->md->sel == NULL || obj->md->sel->dest == NULL){
        Error(m, FUNCNAME, FUNCNAME, LINENUMBER, 
            "Error no source files in module", NULL);
        obj->type.state |= ERROR;
        Return(m, obj);
    }

    obj->src = IoUtil_Annotate(m, path);
    obj->dest = BuildObject_GetDest(m, ctx, obj->md, obj->src);

    struct stat sourceSt;
    struct stat st;
    File_Stat(m, Ifc(m, obj->src, TYPE_STR), &sourceSt);
    if((File_Stat(m, Ifc(m, obj->dest, TYPE_STR), &st) & SUCCESS)
            && (st.st_mtime > sourceSt.st_mtime)){
        obj->type.state |= BUILDOBJ_SATISFIED;
    }
    
    Return(m, obj);
}
