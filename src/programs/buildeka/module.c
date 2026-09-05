#include <external.h>
#include "buildeka_module.h"

static status setDepVars(BuildCtx *ctx, StrVec *key, DirSel *sel){
    /*
    Debug_Push(ctx->m, key);

    status r = READY;
    MemCh *m = ctx->m;
    void *args[5];

    Span *inc = Span_CloneShallow(m, ctx->input.inc);
    Span *moduleInc = Span_Make(m);
    StrVec *path = Table_Get(sel->meta, K(m, "path"));

    StrVec *srcIncPath = StrVec_Copy(m, ctx->src);
    args[0] = IoUtil_PathSep(m);
    args[1] = path;
    args[2] = IoUtil_PathSep(m);
    args[3] = S(m, "include");
    args[4] = NULL;

    StrVec_Anchor(srcIncPath);
    StrVec_AddChain(srcIncPath, args);
    Span_Add(moduleInc, StrVec_StrPrefixed(m, S(m, "-I"), srcIncPath));
    StrVec_Return(srcIncPath);

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
                StrVec_Return(buildDir);
            }
        }
    }

    Table_Set(sel->meta, S(m, "inc"), moduleInc);

    Table *deps = Table_Get(sel->meta, K(m, "dep"));

    ctx->current.staticlibs = Span_Make(m);
    if(deps != NULL){
        Iter it;
        StrVec *srcIncPath = StrVec_Copy(m, ctx->src);
        StrVec_Add(srcIncPath, IoUtil_PathSep(m));
        StrVec_Anchor(srcIncPath);
        Iter_Init(&it, Table_Ordered(m, deps));
        while((Iter_Next(&it) & END) == 0){
            Hashed *h = Iter_Get(&it);
            if(h != NULL){
                StrVec_AddVec(srcIncPath, h->value);
                StrVec_Add(srcIncPath, IoUtil_PathSep(m));
                StrVec_Add(srcIncPath, S(m, "include"));
                StrVec_Add(srcIncPath, IoUtil_PathSep(m));
                Span_Add(moduleInc, StrVec_StrPrefixed(m, S(m, "-I"), srcIncPath));

                StrVec_Return(srcIncPath);
            }
        }

        Iter_Init(&it, Table_Ordered(m, deps));
        while((Iter_Prev(&it) & END) == 0){
            Hashed *h = Iter_Get(&it);
            if(h != NULL){

                DirSel *dsel = Table_Get(ctx->input.dependencies, h->key);
                if(dsel == NULL){
                    void *args[] = {h, Table_Keys(ctx->input.dependencies), NULL};
                    Error(m, FUNCNAME, FILENAME, LINENUMBER,
                        "Dependency expected but not resolved @ of @", args);

                    Return(m, r|ERROR);
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

                Table *linkDeps = Table_Get(dsel->meta, K(m, "link"));
                if(linkDeps != NULL){
                    Table *libMeta = Table_Get(dsel->meta, K(m, "lib"));
                    if(libMeta != NULL){
                        Iter it;
                        Iter_Init(&it, libMeta);
                        while((Iter_Next(&it) & END) == 0){
                            Hashed *h = Iter_Get(&it);
                            if(h != NULL){
                                if(IoUtil_IsStrAbs(h->key)){
                                    path = StrVec_From(m, h->key);
                                }else{
                                    void *ar[] = {
                                        path,
                                        NULL
                                    };
                                    Error(m, FUNCNAME, FILENAME, LINENUMBER,
                                        "Expected absolute path for libdir @", args);

                                    Return(m, ERROR);
                                }
                                Span_Add(ctx->current.liblist, StrVec_StrPrefixed(m, S(m, "-L"), path));
                            }
                        }
                    }


                    Iter _it;
                    Iter_Init(&_it, linkDeps);
                    while((Iter_Next(&_it) & END) == 0){
                        Hashed *h = Iter_Get(&_it);
                        if(h != NULL){
                            Str *value = h->value;
                            Str *s = Str_Make(m, value->length+3);
                            Str_Add(s, (byte *)"-l", 2);
                            Str_Add(s, value->bytes, value->length);
                            Span_Add(ctx->current.liblist, s);
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


    BuildCtx_GenIncFlags(ctx,
        ctx->input.dependencies, Table_Get(sel->meta, K(m, "api")), NULL);

    StrVec *libTarget = Table_Get(sel->meta, K(m, "target"));
    if(libTarget){
        Span_Add(ctx->current.staticlibs, StrVec_Str(m, libTarget)); 
    }

    Span_AddSpan(inc, moduleInc);
    ctx->current.inc = inc;

    Return(ctx->m, r);
    */
    return ZERO;
}

static status skipRecent(BuildCtx *ctx, 
        StrVec *key, DirSel *sel){
    /*
    Debug_Push(ctx->m, key);

    MemCh *m = ctx->m;
    void *args[5];

    struct timespec fileTime;
    if(ctx->current.target != NULL){
        Str *libPathStr = StrVec_Str(m , ctx->current.target);
        if(File_PathExists(m, libPathStr)){
            File_ModTime(m, libPathStr, &fileTime);
            if(Time_Greater(&fileTime, &sel->time)){
                ctx->cli.fields.current[BUILIDER_CLI_ACTION] = K(m, 
                    "Library is recent, skipping");
                BuildCtx_Log(ctx);
                ctx->input.countSources->val.i += ctx->input.totalModuleSources->val.i;

                Return(ctx->m, SUCCESS);
            }
        }
    }

    Return(ctx->m, ZERO);
    */
    return ZERO;
}

static status buildShared(BuildCtx *ctx, StrVec *key, DirSel *sel){
    /*
    MemCh *m = ctx->m;
    Debug_Push(m, key);

    Table *tbl = Table_Get(sel->meta, K(m, "type"));
    if(tbl != NULL && Table_Get(tbl, K(m, "shared")) != NULL){
        Span *objs = Span_Make(m);
        Span_AddSpan(objs, Table_Get(sel->meta, K(m, "destObjs")));
        Table *deps = Table_Get(sel->meta, K(m, "dep"));
        Iter it;
        Iter_Init(&it, Table_Ordered(m, deps));

        i32 count = 0;
        while((Iter_Next(&it) & END) == 0){
            Hashed *h = Iter_Get(&it);
            DirSel *dsel = Table_Get(ctx->input.dependencies, h->key);
            Span_AddSpan(objs, Table_Get(dsel->meta, K(m, "destObjs")));
        }

        StrVec *target = IoUtil_BasePath(m, Table_Get(sel->meta, K(m, "target")));
        StrVec_Add(target, IoUtil_FnameStr(m, key));
        StrVec_AddVec(target, IoPath(m, ".so"));
        
        Span *cmd = Span_Make(m);
        Span_Add(cmd, ctx->tools.cc);
        Span_Add(cmd, S(m, "-shared"));
        Span_Add(cmd, S(m, "-fPIC"));
        Span_Add(cmd, S(m, "-o"));
        Span_Add(cmd, target);
        Span_AddSpan(cmd, objs);

        ProcDets pd;
        ProcDets_Init(m, &pd);
        status re = SubProcess(m, cmd, &pd);
        if(re & ERROR){
            Debug_SetRef(m, cmd);
            void *ar[] = {target, cmd, NULL};
            Fatal(ctx->m, FUNCNAME, FILENAME, LINENUMBER, 
                "Build error for making shared object object @ from cmd @", ar);

            Return(m, ERROR);
        }

        void *ar[] = {target, NULL};
        Out("Shared object built $^0\n", ar);
    }

    Return(m, ZERO);
    */
    return ZERO;
}

static status buildSupporting(BuildCtx *ctx, StrVec *key, DirSel *sel){
    /*
    MemCh *m = ctx->m;
    Debug_Push(m, key);

    void *args[5];

    Table *skips = Table_Get(sel->meta, K(m, "skip"));
    Table *execs = Table_Get(sel->meta, K(m, "exec"));
    Span *destObjs = Span_Make(m);
    Table_Set(sel->meta, K(m, "destObjs"), destObjs);

    ctx->current.source = IoUtil_AbsVec(m, ctx->input.srcPrefix);
    StrVec_Add(ctx->current.source, IoUtil_PathSep(m));
    StrVec_Anchor(ctx->current.source);

    Iter it;
    Iter_Init(&it, sel->dest);
    while((Iter_Next(&it) & END) == 0){

        StrVec *v = Iter_Get(&it);
        Str *fname = Span_Get(v->p, v->p->max_idx);
        if(Table_Get(execs, fname) != NULL){
            continue;
        }

        if(Table_Get(skips, fname) != NULL){
            ctx->cli.fields.current[BUILIDER_CLI_ACTION] = K(m, "Skipping Object");
            ctx->cli.fields.current[BUILIDER_CLI_SOURCE] = v;
            ctx->cli.fields.current[BUILIDER_CLI_DEST] = NULL;
            BuildCtx_Log(ctx);
            continue;
        }

        ctx->input.countSources->val.i++;
        ctx->input.countModuleSources->val.i++;

        IoUtil_Annotate(m, v);
        StrVec *source = StrVec_Make(m);
        StrVec_AddVecAfter(source, v, ctx->input.srcPrefix->p->nvalues+1);
        StrVec_AddVec(ctx->current.source, source);

        StrVec *object = StrVec_Copy(m, source);

        if(Equals(IoUtil_FnameStr(m, object), K(m, "inc.c"))){
            StrVec_Return(ctx->current.dest);
            StrVec_Return(ctx->current.source);
            continue;
        }
    
        if(ctx->type.state & BUILD_SHARED){
            IoUtil_SwapExt(m, object, S(m, "pic-o")); 
        }else{
            IoUtil_SwapExt(m, object, S(m, "o")); 
        }

        StrVec_AddVec(ctx->current.dest, object);
        ctx->current.binDest = NULL;

        Str *outObjDir = StrVec_StrTo(m,
            ctx->current.dest, IoUtil_BasePathAnchor(ctx->current.dest));

        Dir_CheckCreate(m, outObjDir);

        Str *dest = StrVec_Str(m, ctx->current.dest);
        struct timespec fileTime;

        File_ModTime(m, dest, &fileTime);
        if(File_PathExists(m, dest) && Time_Greater(&fileTime, &sel->time)){
            BuildCtx_LinkObject(ctx, key, sel);
            StrVec_Return(ctx->current.dest);
            StrVec_Return(ctx->current.source);
            continue;
        }
        
        Span_Add(destObjs, dest);
        BuildCtx_BuildObject(ctx, key, sel);
        BuildCtx_LinkObject(ctx, key, sel);

        StrVec_Return(ctx->current.dest);
        StrVec_Return(ctx->current.source);
    }

    Return(m, ZERO);
    */
    return ZERO;
}

static status buildExec(BuildCtx *ctx, StrVec *key, DirSel *sel){
    /*
    MemCh *m = ctx->m;
    Debug_Push(m, ZERO);

    void *args[5];

    Table *execs = Table_Get(sel->meta, K(m, "exec"));

    Iter it;
    Iter_Init(&it, execs);
    while((Iter_Next(&it) & END) == 0){
        Hashed *h = Iter_Get(&it);
        if(h != NULL){
            Str *fname = h->value;

            ctx->current.source = IoUtil_AbsVec(m, ctx->input.srcPrefix);

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

            BuildCtx_BuildObject(ctx, (StrVec *)h->key, (DirSel *)h->value);
        }
    }

    Return(m, ZERO);
    */
    return ZERO;
}

status BuildCtx_BuildModule(BuildCtx *ctx, StrVec *name, DirSel *sel){
    status r = READY;
    /*
    MemCh *m = ctx->m;
    Debug_Push(m, name);

    void *args[5];

    if(Table_Get(sel->meta, K(m, "completed")) != NULL){
        Return(m, NOOP);
    }

    setNames(ctx, name, sel);
    Debug_SetRef(m, sel);

    StrVec_Anchor(ctx->current.dest);
    Dir_CheckCreate(m, StrVec_Str(m, ctx->current.dest));

    ctx->cli.fields.current[BUILIDER_CLI_ACTION] = K(m, "Library build");
    ctx->cli.fields.current[BUILIDER_CLI_LIBFILENAME] = ctx->current.targetName;
    ctx->cli.fields.current[BUILIDER_CLI_SOURCE] = name;
    ctx->cli.fields.current[BUILIDER_CLI_DEST] = ctx->current.dest;
    BuildCtx_Log(ctx);

    setDepVars(ctx, name, sel);

    Table *skip = Table_Get(sel->meta, K(m, "skip"));
    if(skip != NULL){
        ctx->input.totalModuleSources->val.i = sel->dest->nvalues -= skip->nvalues;
    }else{
        ctx->input.totalModuleSources->val.i = sel->dest->nvalues;
    }
    ctx->input.countModuleSources->val.i = 0;

    if(skipRecent(ctx, name, sel) & SUCCESS){
        Return(m, NOOP);
    }

    if(ctx->current.target != NULL){
        File_Unlink(m, StrVec_Str(m , ctx->current.target));
    }

    ctx->type.state |= PROCESSING;

    buildSupporting(ctx, name, sel);
    buildShared(ctx, name, sel);
    buildExec(ctx, name, sel);
    struct timespec now;
    Time_Now(&now);
    Table_Set(sel->meta, S(m, "completed"), Time_Wrapped(m, &now));

    Return(m, r);
    */
    return r;
}

void BuildModule_Load(BuildCtx *ctx, BuildModule *md){
    MemCh *m = ctx->m;
    Debug_Push(m, md);
    if(md->config == NULL){
        StrVec *configPath = Clone(m, md->src);
        StrVec_Add(configPath, S(m, "/build.json"));

        StrVec *incPath = Clone(m, md->src);
        StrVec_Add(incPath, S(m, "/inc.c"));

        if(File_PathExists(m, Ifc(m, configPath, TYPE_STR))){
            md->config = Json_FromPath(m, configPath); 
            if(md->config == NULL){
                void *ar[] = {
                    configPath,
                    NULL
                };
                Error(m, FUNCNAME, FILENAME, LINENUMBER,
                    "build.json file not found \\@$", ar);
            }
        }else if(File_PathExists(m, Ifc(m, incPath, TYPE_STR))){
            md->type.state |= BUILDMODULE_INC;
        }else{
            void *ar[] = {md, NULL};
            Error(m, FUNCNAME, FILENAME, LINENUMBER,
                "Neither inc.c or build.json file found for @", ar);
        }
    }

    Str *path = Ifc(m, md->target, TYPE_STR);
    if(File_PathExists(m, path)){
        File_ModTime(m, path, &md->latest); 
    }

    if(md->config != NULL){
        Table *deps = Node_KvFromChild(md->config, K(m, "dependency")); 
        if(deps != NULL){

            if(ctx->deps == NULL){
                ctx->deps = Table_Make(m);
            }

            Iter it;
            Iter_Init(&it, Table_Ordered(m, deps));
            while((Iter_Next(&it) & END) == 0){
                Hashed *h = Iter_Get(&it);
                if(h != NULL){

                    if(Table_Get(ctx->deps, h->key) == NULL){
                        if(Equals(h->value, K(m, "option")) || 
                                Equals(h->value, K(m, "implied-option"))){
                            i32 idx = Span_Has(ctx->options, h->key);
                            if(idx != -1){
                                Abstract *opt = Span_Get(ctx->options, idx);
                                BuildModule *omd = NULL;
                                if(opt->type.of == TYPE_IDENT){
                                    omd = BuildModule_FromIdent(m, ctx, (Ident *)opt);
                                }else{
                                    omd = BuildModule_Make(m, ctx, h->key);
                                }
                                Table_Set(ctx->deps, h->key, omd);
                                BuildModule_Load(ctx, omd);
                            }
                        }else{
                            BuildModule *omd = BuildModule_Make(m, ctx, h->key);
                            Table_Set(ctx->deps, h->key, omd);
                            BuildModule_Load(ctx, omd);
                        }
                    }
                }
            }
        }
    }

    ReturnVoid(m);
}

BuildModule *BuildModule_Make(MemCh *m, BuildCtx *ctx, StrVec *name){
    BuildModule *md = MemCh_AllocOf(m, sizeof(BuildModule), TYPE_BUILD_MODULE);
    md->type.of = TYPE_BUILD_MODULE;
    md->name = name;

    Debug_Push(m, md);

    md->src = Clone(m, ctx->src);
    StrVec *domain = domain = Sv(m, "mod");

    IoUtil_AddVec(m, md->src, domain);
    IoUtil_AddVec(m, md->src, name);

    StrVec *target = Sv(m, "libcnk-");
    StrVec_AddVec(target, Clone(m, md->name));
    StrVec_AddVec(target, Clone(m, domain));

    md->targetName = Clone(m, target);
    md->target = Clone(m, ctx->dir);

    IoUtil_AddVec(m, md->target, Sv(m, "lib"));
    IoUtil_AddVec(m, md->target, target);
    IoUtil_AddVec(m, md->target, target);
    StrVec_AddVec(md->target, Sv(m, ".a"));


    Return(m, md);
}

BuildModule *BuildModule_FromIdent(MemCh *m, BuildCtx *ctx, Ident *ident){
    BuildModule *md = MemCh_AllocOf(m, sizeof(BuildModule), TYPE_BUILD_MODULE);
    md->type.of = TYPE_BUILD_MODULE;
    md->name = StrVec_From(m, Ident_NameStr(m, ident));

    Debug_Push(m, md);

    md->src = Clone(m, ctx->src);

    StrVec *domain = StrVec_From(m, Ident_DomainStr(m, ident));
    if(!Equals(domain, K(m, "programs"))){
        domain = Sv(m, "mod");
    }

    IoUtil_AddVec(m, md->src, domain);
    if(ident->value == NULL){
        IoUtil_AddVec(m, md->src, StrVec_From(m, Ident_NameStr(m, ident)));
    }else{
        IoUtil_AddVec(m, md->src, StrVec_From(m, Ident_ValueStr(m, ident)));
    }

    StrVec *target = Sv(m, "libcnk-");
    StrVec_AddVec(target, Clone(m, md->name));
    StrVec_AddVec(target, Clone(m, domain));

    md->targetName = Clone(m, target);
    md->target = Clone(m, ctx->dir);

    IoUtil_AddVec(m, md->target, Sv(m, "lib"));
    IoUtil_AddVec(m, md->target, target);
    IoUtil_AddVec(m, md->target, target);
    StrVec_AddVec(md->target, Sv(m, ".a"));

    Return(m, md);
}
