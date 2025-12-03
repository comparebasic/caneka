#include <external.h>
#include "cnkbuild_module.h"

static status setNames(BuildCtx *ctx, StrVec *key, DirSelector *sel){
    DebugStack_Push(NULL, ZERO);

    ctx->current.name = IoUtil_FnameStr(m, key);
    ctx->current.targetName = StrVec_Str(m,
        StrVec_StrPrefixed(m,
            S(m, "libcnk"), ctx->current.name));

    if(libSourceTotal > 0){
        ctx->current.target = StrVec_Copy(m, ctx->input.buildDir);
        args[0] = IoUtil_PathSep(m);
        args[1] = ctx->current.targetName;
        args[2] = IoUtil_PathSep(m);
        args[3] = ctx->current.targetName;
        args[4] = S(m, ".a");
        args[5] = NULL;
        StrVec_AddChain(ctx->current.target, args);

        Table_Set(sel->meta, S(m, "target"), StrVec_Copy(m, ctx->current.target));
    }else{
        ctx->current.targetName = NULL;
        ctx->current.target = NULL;
    }

    ctx->current.dest = StrVec_Copy(m, ctx->input.buildDir);
    args[0] = IoUtil_PathSep(m);
    args[1] = targetName;
    args[2] = IoUtil_PathSep(m);
    args[4] = NULL;
    StrVec_AddChain(ctx->current.dest, args);

    DebugStack_Pop();
    return ZERO;
}

static status setDepVars(BuildCtx *ctx, StrVec *key, DirSelector *sel){
    DebugStack_Push(NULL, ZERO);

    Span *inc = Span_CloneShallow(m, ctx->input.inc);
    Span *moduleInc = Span_Make(m);

    StrVec *srcIncPath = StrVec_Copy(m, ctx->src);
    args[0] = IoUtil_PathSep(m);
    args[1] = key;
    args[2] = IoUtil_PathSep(m);
    args[3] = S(m, "include");
    args[4] = NULL;

    StrVec_Anchor(srcIncPath);
    StrVec_AddChain(srcIncPath, args);
    Span_Add(moduleInc, StrVec_StrPrefixed(m, S(m, "-I"), srcIncPath));
    StrVec_PopToAnchor(srcIncPath);

    StrVec_ReturnToAnchor(ctx->current.dest);

    Table *incMeta = Table_Get(sel->meta, S(m, "include"));
    if(incMeta != NULL){
        StrVec *buildDir = StrVec_Copy(m,ctx->input.buildDir); 
        StrVec_Add(buildDir, IoUtil_PathSep(m));
        Iter it;
        Iter_Init(&it, incMeta);
        StrVec_Anchor(buildDir);
        while((Iter_Next(&it) & END) == 0){
            Hashed *h = Iter_Get(&it);
            if(h != NULL){
                StrVec *path = NULL;
                if(IoUtil_IsStrAbs(h->key)){
                    path = StrVec_From(m, h->key);
                }else{
                    StrVec_AddVec(buildDir, key);
                    StrVec_Add(buildDir, IoUtil_PathSep(m));
                    StrVec_Add(buildDir, h->key);

                    path = buildDir;
                }
                Span_Add(moduleInc, StrVec_StrPrefixed(m, S(m, "-I"), path));
                StrVec_PopToAnchor(buildDir);
            }
        }
    }

    Table_Set(sel->meta, S(m, "inc"), moduleInc);

    Span_AddSpan(inc, moduleInc);

    Table *deps = Table_Get(sel->meta, K(m, "dep"));

    Span *modlist = Span_Make(m);
    Span_Add(modlist, h->key);
    if(deps != NULL){
        Iter it;
        Iter_Init(&it, Table_Ordered(m, deps));
        while((Iter_Next(&it) & END) == 0){
            Hashed *h = Iter_Get(&it);
            if(h != NULL){
                Span_Add(modlist, h->key);
            }
        }
    }

    BuildCtx_GenInclude(ctx, modlist);

    ctx->current.staticlibs = Span_Make(m);

    if(deps != NULL){
        Iter it;
        Iter_Init(&it, Table_Ordered(m, deps));
        while((Iter_Prev(&it) & END) == 0){
            Hashed *h = Iter_Get(&it);
            if(h != NULL){
                DirSelector *dsel = Table_Get(ctx->input.dependencies, h->key); 
                if(dsel == NULL){
                    void *args[] = {h->key, NULL};
                    Error(m, FUNCNAME, FILENAME, LINENUMBER,
                        "Dependency expected but not resolved @", args);
                    return r|ERROR;
                }
                Span *depInc = Table_Get(dsel->meta, K(m, "inc"));
                if(depInc != NULL){
                    Span_AddSpan(inc, depInc);
                }
                Table *staticDeps = Table_Get(dsel->meta, K(m, "static"));
                if(staticDeps != NULL){
                    Iter _it;
                    Iter_Init(&_it, staticDeps);
                    while((Iter_Next(&_it) & END) == 0){
                        Hashed *h = Iter_Get(&_it);
                        if(h != NULL){
                            void *ar[] = {h->value, NULL};
                            Span_Add(ctx->current.staticlibs, StrVec_Str(m, h->value)); 
                        }
                    }
                }
                StrVec *libTarget = Table_Get(dsel->meta, K(m, "target"));
                if(libTarget){
                    void *ar[] = {libTarget, NULL};
                    Span_Add(ctx->current.staticlibs, StrVec_Str(m, libTarget)); 
                }
            }
        }
    }
    StrVec *libTarget = Table_Get(sel->meta, K(m, "target"));
    if(libTarget){
        Span_Add(ctx->current.staticlibs, StrVec_Str(m, libTarget)); 
    }

    ctx->current.inc = inc;

    DebugStack_Pop();
    return ZERO;
}

static microTime skipRecent(BuildCtx *ctx, 
        StrVec *key, DirSelector *sel, microTime *modified){
    DebugStack_Push(NULL, ZERO);
    if(ctx->current.target != NULL){
        Str *libPathStr = StrVec_Str(m , ctx->current.target);
        if(File_PathExists(m, libPathStr)){
            *modified = File_ModTime(m, libPathStr);
            if(*modified > sel->time){
                ctx->cli.fields.current[BUILIDER_CLI_ACTION] = K(m, 
                    "Library is recent, skipping");
                LogOut(ctx);
                ctx->input.countSources->val.i += ctx->input.totalModuleSources->val.i;
                return SUCCESS;
            }
        }
    }

    DebugStack_Pop();
    return ZERO;
}

static status buildSupporting(BuildCtx *ctx, StrVec *key, DirSelector *sel){
    DebugStack_Push(NULL, ZERO);

    Table *skips = Table_Get(sel->meta, K(m, "skip"));
    Table *execs = Table_Get(sel->meta, K(m, "exec"));

    ctx->current.source = IoUtil_AbsVec(m, ctx->input.srcPrefix);
    StrVec_Add(ctx->current.source, IoUtil_PathSep(m));
    StrVec_Anchor(ctx->current.source);

    Iter it;
    Iter_Init(&it, sel->dest);
    while((Iter_Next(&it) & END) == 0){

        StrVec *v = Iter_Get(&it);
        Str *fname = Span_Get(v->p, v->p->max_idx);
        if(Table_Get(execs, fname) != NULL){
            /* handled below */
            continue;
        }

        if(Table_Get(skips, fname) != NULL){
            ctx->cli.fields.current[BUILIDER_CLI_ACTION] = K(m, "Skipping Object");
            ctx->cli.fields.current[BUILIDER_CLI_SOURCE] = v;
            ctx->cli.fields.current[BUILIDER_CLI_DEST] = NULL;
            LogOut(ctx);
            continue;
        }

        ctx->input.countSources->val.i++;
        ctx->input.countModuleSources->val.i++;

        IoUtil_Annotate(m, v);
        StrVec *source = StrVec_Make(m);
        StrVec_AddVecAfter(source, v, ctx->input.srcPrefix->p->nvalues+1);
        StrVec_AddVec(ctx->current.source, source);

        StrVec *object = IoUtil_SwapExt(m, source, K(m, ".c"), K(m, ".o")); 
        StrVec_AddVec(ctx->current.dest, object);
        ctx->current.binDest = NULL;

        Str *outObjDir = StrVec_StrTo(m,
            ctx->current.dest, IoUtil_BasePathAnchor(ctx->current.dest));

        Dir_CheckCreate(m, outObjDir);

        Str *dest = StrVec_Str(m, ctx->current.dest);

        if(File_PathExists(m, dest) && File_ModTime(m, dest) > modified){
            BuildCtx_LinkObject(ctx, (StrVec *)h->key, (DirSelector *)h->value);
            StrVec_ReturnToAnchor(ctx->current.dest);
            StrVec_ReturnToAnchor(ctx->current.source);
            continue;
        }

        BuildCtx_BuildObject(ctx, (StrVec *)h->key, (DirSelector *)h->value);
        BuildCtx_LinkObject(ctx, (StrVec *)h->key, (DirSelector *)h->value);

        StrVec_ReturnToAnchor(ctx->current.dest);
        StrVec_ReturnToAnchor(ctx->current.source);
    }

    DebugStack_Pop();
    return ZERO;
}

static status buildExec(BuildCtx *ctx, StrVec *key, DirSelector *sel){
    DebugStack_Push(NULL, ZERO);

    Table *execs = Table_Get(sel->meta, K(m, "exec"));

    Iter_Init(&it, execs);
    while((Iter_Next(&it) & END) == 0){
        Hashed *h = Iter_Get(&it);
        if(h != NULL){
            Str *fname = h->value;

            ctx->input.countSources->val.i++;
            ctx->input.countModuleSources->val.i++;

            StrVec *source = StrVec_From(m, fname);
            IoUtil_Annotate(m, source);
            args[0] = IoUtil_PathSep(m);
            args[1] = key;
            args[2] = IoUtil_PathSep(m);
            args[3] = source;
            args[4] = NULL;
            StrVec_AddChain(ctx->current.source, args);

            Str *binName = Str_Clone(m, fname);
            Str_Decr(binName, 2);
            StrVec *bin = StrVec_From(m, binName);
            if(Equals(binName, S(m, "main"))){
                bin = StrVec_From(m, Span_Get(key->p, key->p->max_idx));
            }

            StrVec_AddVec(ctx->current.dest, bin);
            ctx->current.binDest = StrVec_Copy(m, ctx->input.buildDir);
            args[0] = IoUtil_PathSep(m);
            args[1] = S(m, "bin");
            args[2] = IoUtil_PathSep(m);
            args[3] = bin;
            args[4] = NULL;
            StrVec_AddChain(ctx->current.binDest, args);

            BuildCtx_BuildObject(ctx, (StrVec *)h->key, (DirSelector *)h->value);

            StrVec_ReturnToAnchor(ctx->current.source);
        }
    }

    DebugStack_Pop();
    return ZERO;
}

status BuildCtx_BuildModule(BuildCtx *ctx, StrVec *key, DirSelector *sel){
    DebugStack_Push(NULL, ZERO);
    status r = READY;
    MemCh *m = ctx->m;
    void *args[5];

    if(Table_Get(sel->meta, K(m, "completed")) != NULL){
        return NOOP;
    }

    Table *skips = Table_Get(sel->meta, K(m, "skip"));
    Table *execs = Table_Get(sel->meta, K(m, "exec"));

    i32 libSourceTotal = sel->dest->nvalues;
    if(skips != NULL){ libSourceTotal -= skips->nvalues; }
    if(execs != NULL){ libSourceTotal -= execs->nvalues; }

    setNames(ctx, key, sel);
    DebugStack_SetRef(ctx->current.targetName, ctx->current->targetName->type.of);

    StrVec_Anchor(ctx->current.dest);
    Dir_CheckCreate(m, StrVec_Str(m, ctx->current.dest));

    ctx->cli.fields.current[BUILIDER_CLI_ACTION] = K(m, "Library build");
    ctx->cli.fields.current[BUILIDER_CLI_LIBFILENAME] = ctx->current.targetName;
    ctx->cli.fields.current[BUILIDER_CLI_SOURCE] = h->key;
    ctx->cli.fields.current[BUILIDER_CLI_DEST] = ctx->current.dest;
    LogOut(ctx);

    setDepVars(ctx, key, sel);

    Table *skip = Table_Get(sel->meta, K(m, "skip"));
    if(skip != NULL){
        ctx->input.totalModuleSources->val.i = sel->dest->nvalues -= skip->nvalues;
    }else{
        ctx->input.totalModuleSources->val.i = sel->dest->nvalues;
    }
    ctx->input.countModuleSources->val.i = 0;

    microTime modified = 0;
    if(skipRecent(ctx, key, sel, &modified) & SUCCESS){
        return NOOP;
    }

    if(ctx->current.target != NULL){
        File_Unlink(m, StrVec_Str(m , ctx->current.target));
    }

    ctx->type.state |= PROCESSING;

    buildSupporting(ctx, key, sel);
    buildExec(ctx, key, sel);
    Table_Set(sel->meta, S(m, "completed"), I64_Wrapped(m, MicroTime_Now()));

    DebugStack_Pop();
    return r;
}

