#include <external.h>
#include "buildeka_module.h"

static DirSel *BuildModule_makeDirSel(MemCh *m, BuildCtx *ctx, Str *ext, StrVec *path){
    DirSel *sel = DirSel_Make(m,
        ext, NULL, DIR_SELECTOR_MTIME_ALL|DIR_SELECTOR_NODIRS);

    StrVec *base = StrVec_Copy(m, path);
    StrVec_Add(base, IoUtil_PathSep(m));
    StrVec_Add(base, S(m, "option"));

    sel->source = Span_Make(m);
    Span_Add((Span *)sel->source, Ifc(m, base, TYPE_STR));
    sel->type.state |= DIR_SELECTOR_INVERT;
    return sel;
}

static void BuildModule_makeDestDir(MemCh *m, BuildCtx *ctx, BuildModule *md){
    StrVec *path = IoUtil_BasePath(m, md->target);
    IoUtil_AddVec(m, path, Sv(m, "object"));
    Dir_CheckCreate(m, Ifc(m, path, TYPE_STR));
}

void BuildModule_SetFlags(BuildCtx *ctx, BuildModule *md){
    MemCh *m = md->m;
    Debug_Push(m, md);
    md->flags = Span_Make(m);
    void *args[4];

    args[0] = ctx->dest;
    args[1] = NULL;
    Span_Add(md->flags, S(m, "-I"));
    Span_Add(md->flags, Fmt_ToStrVec(m, "$/include/", args));

    args[0] = ctx->src;
    args[1] = NULL;
    Span_Add(md->flags, S(m, "-I"));
    Span_Add(md->flags, Fmt_ToStrVec(m, "$/api/include/", args));

    Iter it;
    Iter_Init(&it, ctx->depsOrdered);
    while((Iter_Next(&it) & END) == 0){
        BuildModule *omd = (BuildModule *)Iter_Get(&it);
        args[0] = omd->src;
        args[1] = omd->name;
        args[2] = NULL;
        Span_Add(md->flags, S(m, "-I"));
        Span_Add(md->flags, Fmt_ToStrVec(m, "$/include/", args));
    }

    Iter_Init(&it, ctx->options);
    while((Iter_Next(&it) & END) == 0){
        Abstract *a = Iter_Get(&it);
        Str *opt = NULL;
        if(a->type.of == TYPE_STR){
            opt = (Str *)a;
        }else if(a->type.of == TYPE_IDENT){
            Ident *ident = (Ident *)a;
            opt = (Str *)Ifc(m, ident->name, TYPE_STR);
        }
        if(opt != NULL){
            opt = Str_ToUpper(m, opt);
            args[0] = opt;
            args[1] = NULL;
            Span_Add(md->flags, S(m, "-D"));
            Span_Add(md->flags, Fmt_ToStrVec(m, "CNKOPT_$", args));
        }
    }

    ReturnVoid(m);
}

void BuildModule_BuildCurrent(BuildCtx *ctx){

    BuildModule *md = (BuildModule *)Iter_Get(&ctx->current.moduleIt);
    if(md == NULL){
        Error(ctx->m, FUNCNAME, FUNCNAME, LINENUMBER, 
            "Error no module found as current in Iter", NULL);
        ctx->type.state |= ERROR;
        ReturnVoid(ctx->m);
    }

    MemCh *m = md->m;
    Debug_Push(m, md);
    BuildModule_SetFlags(ctx, md);
    BuildModule_makeDestDir(m, ctx, md);

    void *ar[] = {
        md->name,
        Type_StateVec(m, md->type.of, md->type.state),
        md->targetName, 
        Time_ToRStr(m, &md->latest),
        I32_Wrapped(m, md->metrics.sources),
        md->src,
        md->target,
        NULL
    };
    Out("^p.Building @/@ -> ^D.$^d. latest(@) files:@ -> \n  $ -> $^0\n", ar);

    if(md->sel == NULL || md->sel->dest == NULL){
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "Empty DirSel no file count to build", NULL);
        ReturnVoid(m);
    }

    Str *targetPathS = Ifc(m, md->target, TYPE_STR);
    if(File_PathExists(m, targetPathS)){
        File_Unlink(m, targetPathS);
    }

    Iter_Init(&ctx->current.sourcesIt, md->sel->dest);
    while((Iter_Next(&ctx->current.sourcesIt) & END) == 0){
        StrVec *v = Iter_Get(&ctx->current.sourcesIt);
        BuildObject *obj = BuildObject_Current(m, ctx);
        if(v->type.state & MORE){
            if((obj->type.state & BUILDOBJ_SATISFIED) == 0){
                void *ar[] = {
                    obj,
                    NULL
                };
                Out("^c.Building @^0\n", ar);
                BuildObject_Build(m, ctx, obj);
            }
        }
        BuildObject_Link(m, ctx, obj);
        md->metrics.built++;
        ctx->metrics.built++;
    }

    ctx->metrics.modulesBuilt++;

    ReturnVoid(m);
}

void BuildModule_SetStatus(BuildCtx *ctx, BuildModule *md){
    MemCh *m = md->m;
    if(md->sel == NULL){
        return;
    }

    void *ar[] = {md->config, NULL};
    Out("^y.Config @^0\n", ar);

    Span *exec = Node_SpanFromChild(md->config, K(m, "exec")); 
    if(exec != NULL){
        md->execTbl = Table_Make(m);

        void *ar[] = {
            md->name,
            exec,
            NULL
        };
        Out("^p. exec @ @^0\n", ar);
    }

    Iter it;
    Iter_Init(&it, md->sel->dest);
    while((Iter_Next(&it) & END) == 0){
        StrVec *v = Iter_Get(&it);
        StrVec *path = IoUtil_Annotate(m, v);

        Hashed *h = NULL; 
        if(md->execTbl != NULL && (h = Table_Get(md->execTbl, path)) != NULL){
            v->type.state |= LAST;
        }

        StrVec *out = BuildObject_GetDest(m, ctx, md, path);

        struct stat sourceSt;
        struct stat st;
        File_Stat(m, Ifc(m, v, TYPE_STR), &sourceSt);
        if((File_Stat(m, Ifc(m, out, TYPE_STR), &st) & ERROR)
                || (sourceSt.st_mtime > st.st_mtime)){
            v->type.state |= MORE;
        }else{
            md->metrics.built++;
        }
    }

    if(md->metrics.built == md->metrics.sources){
        md->type.state |= BUILDMODULE_SATISFIED;
    }
}

void BuildModule_Gather(MemCh *m, BuildCtx *ctx, BuildModule *md){
    struct timespec hdrLatest;
    Iter it;

    DirSel *hdrSel = BuildModule_makeDirSel(m, ctx, S(m, ".h"), md->src);
    StrVec *hpath = Clone(m, md->src);
    IoUtil_AddVec(m, hpath, Sv(m, "include"));
    Dir_GatherFilterDir(m, Ifc(m, hpath, TYPE_STR), hdrSel);

    Span *optionPaths = Span_Make(m);
    Iter_Init(&it, ctx->optionNames);
    while((Iter_Next(&it) & END) == 0){
        StrVec *v = Clone(m, md->src);
        IoUtil_AddVec(m, v, Sv(m, "include"));
        IoUtil_AddVec(m, v, Sv(m, "option"));
        IoUtil_AddVec(m, v, StrVec_From(m, Iter_Get(&it)));

        Span_Add(optionPaths, Ifc(m, v, TYPE_STR));
    }

    hdrSel->source = optionPaths;
    hdrSel->type.state &= ~DIR_SELECTOR_INVERT;
    Dir_GatherFilterDir(m, Ifc(m, hpath, TYPE_STR), hdrSel);
    
    md->sel = BuildModule_makeDirSel(m, ctx, S(m, ".c"), md->src);
    Dir_GatherFilterDir(m, Ifc(m, md->src, TYPE_STR), md->sel);

    optionPaths = Span_Make(m);
    Iter_Init(&it, ctx->optionNames);
    while((Iter_Next(&it) & END) == 0){
        StrVec *v = Clone(m, md->src);
        IoUtil_AddVec(m, v, Sv(m, "option"));
        IoUtil_AddVec(m, v, StrVec_From(m, Iter_Get(&it)));

        Span_Add(optionPaths, Ifc(m, v, TYPE_STR));
    }

    md->sel->source = optionPaths;
    md->sel->type.state &= ~DIR_SELECTOR_INVERT;
    Dir_GatherFilterDir(m, Ifc(m, md->src, TYPE_STR), md->sel);

    if(Time_Greater(&hdrSel->time, &md->sel->time)){
        memcpy(&md->latest, &hdrLatest, sizeof(struct timespec));
        md->type.state |= BUILDMODULE_HEADER_CHANGE;
    }else{
        memcpy(&md->latest, &md->sel->time, sizeof(struct timespec));
        md->type.state |= BUILDMODULE_SOURCE_CHANGE;
    }
}

void BuildModule_Load(BuildCtx *ctx, BuildModule *md){
    MemCh *m = md->m;
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
                        BuildModule *omd = NULL;
                        if(Equals(h->value, K(m, "option")) || 
                                Equals(h->value, K(m, "implied-option"))){
                            i32 idx = Span_Has(ctx->options, h->key);
                            if(idx != -1){
                                Abstract *opt = Span_Get(ctx->options, idx);
                                if(opt->type.of == TYPE_IDENT){
                                    omd = BuildModule_FromIdent(MemCh_Make(),
                                        ctx, (Ident *)opt);
                                }else{
                                    omd = BuildModule_Make(MemCh_Make(), ctx, h->key);
                                }
                                Table_Set(ctx->deps, h->key, omd);
                                BuildModule_Load(ctx, omd);
                            }
                        }else{
                            omd = BuildModule_Make(MemCh_Make(), ctx, h->key);
                            Table_Set(ctx->deps, h->key, omd);
                            BuildModule_Load(ctx, omd);
                        }
                    }
                }
            }
        }
    }

    BuildModule_Gather(m, ctx, md);

    if(md->sel != NULL && md->sel->dest != NULL ){
        md->metrics.sources = md->sel->dest->nvalues;
    }

    Str *targetStr = Ifc(m, md->target, TYPE_STR);
    struct timespec targetModified = {0, 0};
    if(File_PathExists(m, targetStr)){
        File_ModTime(m, targetStr, &targetModified);
        if(targetModified.tv_sec > 0 && Time_Greater(&targetModified, &md->latest)){
            md->type.state |= BUILDMODULE_SATISFIED;
            md->metrics.built = md->metrics.sources;
        }else{
            BuildModule_SetStatus(ctx, md); 
        }
    }else{
        BuildModule_SetStatus(ctx, md); 
    }

    ReturnVoid(m);
}

BuildModule *BuildModule_Make(MemCh *m, BuildCtx *ctx, StrVec *name){
    BuildModule *md = MemCh_AllocOf(m, sizeof(BuildModule), TYPE_BUILD_MODULE);
    md->type.of = TYPE_BUILD_MODULE;
    md->m = m;
    md->name = name;

    Debug_Push(m, md);

    md->src = Clone(m, ctx->src);
    md->local = StrVec_Make(m);
    StrVec *domain = domain = Sv(m, "mod");

    IoUtil_AddVec(m, md->local, domain);
    IoUtil_AddVec(m, md->local, name);
    IoUtil_AddVec(m, md->src, md->local);

    StrVec *target = Sv(m, "libcaneka-");
    StrVec_AddVec(target, Clone(m, md->name));
    StrVec_Add(target, S(m, "-"));
    StrVec_AddVec(target, Clone(m, domain));

    md->targetName = Clone(m, target);
    md->target = Clone(m, ctx->dest);

    IoUtil_AddVec(m, md->target, Sv(m, "lib"));
    IoUtil_AddVec(m, md->target, target);
    IoUtil_AddVec(m, md->target, target);
    StrVec_AddVec(md->target, Sv(m, ".a"));

    md->m->level++;

    Return(m, md);
}

BuildModule *BuildModule_FromIdent(MemCh *m, BuildCtx *ctx, Ident *ident){
    BuildModule *md = MemCh_AllocOf(m, sizeof(BuildModule), TYPE_BUILD_MODULE);
    md->type.of = TYPE_BUILD_MODULE;
    md->m = m;
    md->name = StrVec_From(m, Ident_NameStr(m, ident));

    Debug_Push(m, md);

    md->src = Clone(m, ctx->src);
    md->local = StrVec_Make(m);

    StrVec *domain = StrVec_From(m, Ident_DomainStr(m, ident));
    if(!Equals(domain, K(m, "program"))){
        IoUtil_AddVec(m, md->local, Sv(m, "mod"));
    }

    IoUtil_AddVec(m, md->local, domain);
    if(ident->value == NULL){
        IoUtil_AddVec(m, md->local, StrVec_From(m, Ident_NameStr(m, ident)));
    }else{
        IoUtil_AddVec(m, md->local, StrVec_From(m, Ident_ValueStr(m, ident)));
    }

    IoUtil_AddVec(m, md->src, md->local);

    StrVec *target = Sv(m, "libcaneka-");
    StrVec_AddVec(target, Clone(m, md->name));
    StrVec_Add(target, S(m, "-"));
    StrVec_AddVec(target, Clone(m, domain));

    md->targetName = Clone(m, target);
    md->target = Clone(m, ctx->dest);

    IoUtil_AddVec(m, md->target, Sv(m, "lib"));
    IoUtil_AddVec(m, md->target, target);
    IoUtil_AddVec(m, md->target, target);
    StrVec_AddVec(md->target, Sv(m, ".a"));

    md->m->level++;

    Return(m, md);
}
