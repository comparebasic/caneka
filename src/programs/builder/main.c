#include <external.h>
#include <caneka.h>
#include "module.h"

static boolean _quiet = FALSE;

static status parseDependencies(BuildCtx *ctx, StrVec *key, StrVec *path){
    void *args[5];
    MemCh *m = ctx->m;

    i32 anchor = StrVec_AddVecAfter(   
        ctx->current.source,
        path,
        ctx->input.srcPrefix->p->nvalues);

    StrVec_Add(ctx->current.source, IoUtil_PathSep(m));

    Str *pathS = StrVec_Str(m, path);
    DirSelector *sel = NULL;
    if(Dir_Exists(m, pathS) & SUCCESS){
        sel = DirSelector_Make(m,
            S(m, ".c"), NULL, DIR_SELECTOR_MTIME_ALL|DIR_SELECTOR_NODIRS);
        Dir_GatherSel(m, pathS, sel);
        StrVec *name = StrVec_Make(m);
        StrVec_AddVecAfter(name, path, ctx->input.srcPrefix->p->nvalues+1);

        ctx->input.totalModules->val.value++;
        Table_Set(ctx->input.dependencies, name, sel);

        if(ctx->modified < sel->time){
            ctx->modified = sel->time;
        }

        ctx->input.totalSources->val.value += sel->dest->nvalues;
        if(!Equals(key, path)){
            Span *p = NULL;
            Str *meta = S(m, "choice");
            if((p = Table_Get(sel->meta, meta)) == NULL){
                p = Span_Make(m);
                Table_Set(sel->meta, meta, p);
            }
            key->type.state |= BUILD_CHOICE;
            Span_Add(p, name);
        }
    }else{
        args[0] = path;
        args[1] = ctx;
        args[2] = NULL;
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "Dependency not found @ for @", args);
        return ERROR;
    }

    StrVec_Add(ctx->current.source, K(m, "dependencies.txt"));

    Buff *bf = Buff_Make(m, ZERO|BUFF_SLURP);
    bf->type.state |= NOOP;
    File_Open(bf, StrVec_Str(m, ctx->current.source), O_RDONLY);
    if(bf->type.state & ERROR){
       return NOOP; 
    }
    Buff_Read(bf);
    File_Close(bf);

    StrVec_PopTo(ctx->current.source, anchor);

    Str *shelf = Str_Make(m, STR_DEFAULT);
    Cursor *curs = Cursor_Make(m, bf->v);
    Str *label = NULL;
    Str *name = NULL;
    while((Cursor_NextByte(curs) & END) == 0){
        if(*curs->ptr == '\n'){
            if(label != NULL && label->length > 0 ||
                    name != NULL && name->length > 0){
                if(Equals(label, K(m, "choice"))){
                    if(name != NULL && name->length > 0){
                        key = StrVec_From(m, Str_Clone(m, name));
                    }
                }else{
                    Str *meta = label;
                    if(Equals(label, K(m, "exec"))){
                        meta->type.state |= BUILD_EXEC; 
                        ctx->input.totalSources->val.value--;
                        Single *sg = NULL;
                        if((sg = Table_Get(sel->meta, K(m, "skip-count"))) == NULL){
                            sg = I64_Wrapped(m, 0);
                            Table_Set(sel->meta, K(m, "skip-count"), sg);
                        }
                        sg->val.value++;
                    }else if(Equals(label, K(m, "static"))){
                        meta->type.state |= BUILD_STATIC; 
                    }else if(Equals(label, K(m, "link"))){
                        meta->type.state |= BUILD_LINK; 
                    }else if(Equals(label, K(m, "skip"))){
                        meta->type.state |= BUILD_SKIP; 
                        ctx->input.totalSources->val.value--;
                        Single *sg = NULL;
                        if((sg = Table_Get(sel->meta, K(m, "skip-count"))) == NULL){
                            sg = I64_Wrapped(m, 0);
                            Table_Set(sel->meta, K(m, "skip-count"), sg);
                        }
                        sg->val.value++;
                    }else if(Equals(label, K(m, "include"))){
                        meta->type.state |= BUILD_INCLUDE; 
                    }
                    Span *p = NULL;
                    if((p = Table_Get(sel->meta, meta)) == NULL){
                        p = Span_Make(m);
                        Table_Set(sel->meta, meta, p);
                    }
                    Span_Add(p, StrVec_From(m, shelf));
                    label = NULL;
                    name = NULL;
                    shelf = Str_Make(m, STR_DEFAULT);
                    continue;
                }
            }else{
                key = NULL;
            }

            path = StrVec_Copy(m, ctx->input.srcPrefix);
            StrVec_Add(path, IoUtil_PathSep(m));
            StrVec_Add(path, shelf);
            IoUtil_Annotate(ctx->m, path);
            if(key == NULL){
                key = path;
            }

            parseDependencies(ctx, key, path);
            label = NULL;
            name = NULL;
            shelf = Str_Make(m, STR_DEFAULT);
        }else if(*curs->ptr == '@'){
            name = shelf;
            shelf = Str_Make(m, STR_DEFAULT);
        }else if(*curs->ptr == '='){
            label = shelf;
            shelf = Str_Make(m, STR_DEFAULT);
        }else{
            Str_Add(shelf, curs->ptr, 1);
        }
    }

    StrVec_PopTo(ctx->current.source, anchor);
    return ZERO;
}

static status genInclude(BuildCtx *ctx){
    void *args[5];
    MemCh *m = ctx->m;

    i32 anchor = StrVec_Add(ctx->current.dest, IoUtil_PathSep(m));
    StrVec_Add(ctx->current.dest, S(m, "include"));
    StrVec_Add(ctx->current.dest, IoUtil_PathSep(m));
    Dir_CheckCreate(m, StrVec_Str(m, ctx->current.dest));
    StrVec_Add(ctx->current.dest, S(m, "caneka.h"));

    Str *fname = StrVec_Str(m, ctx->current.dest);
    StrVec *absSrc = IoUtil_AbsVec(m, ctx->input.srcPrefix);
    Buff *bf = Buff_Make(m, BUFF_CLOBBER|BUFF_UNBUFFERED);
    File_Open(bf, fname, O_CREAT|O_WRONLY);

    ctx->cli.fields.current[BUILIDER_CLI_DEST] = fname;

    i32 srcAnchor = StrVec_Add(absSrc, IoUtil_PathSep(m));
    StrVec_Add(absSrc, S(m, "external.h"));
    args[0] = absSrc;
    args[1] = NULL;
    Fmt(bf, "/* This file generated by cnkbuild */\n\n#include \"$\"\n", args);
    StrVec_PopTo(absSrc, srcAnchor);

    ctx->input.countModules->val.value = 0;
    Iter it;
    Iter_Init(&it, Table_Ordered(ctx->m, ctx->input.dependencies));
    while((Iter_Prev(&it) & END) == 0){
        Hashed *h = Iter_Get(&it);
        if(h != NULL){
            args[0] = h->key; 
            args[1] = ctx->current.dest;

            i32 srcAnchor = StrVec_Add(absSrc, IoUtil_PathSep(m));
            StrVec_AddVec(absSrc, h->key);
            StrVec_Add(absSrc, IoUtil_PathSep(m));
            StrVec_Add(absSrc, S(m, "module.h"));

            ctx->input.countModules->val.value = it.p->nvalues - it.idx;
            ctx->cli.fields.current[BUILIDER_CLI_ACTION] = K(m, "Writing include");
            ctx->cli.fields.current[BUILIDER_CLI_LIBFILENAME] = K(m, "Global");
            ctx->cli.fields.current[BUILIDER_CLI_SOURCE] = absSrc;
            LogOut(ctx);

            args[0] = h->key;
            args[1] = absSrc;
            args[2] = NULL;
            Fmt(bf, "\n/* module $ */\n#include \"$\"\n", args);
            StrVec_PopTo(absSrc, srcAnchor);
        }
    }

    File_Close(bf);
    StrVec_PopTo(ctx->current.dest, anchor);
    return ZERO;
}

static status buildExec(BuildCtx *ctx,
        boolean force, Str *destDir, Str *lib, Executable *target){
    /*
    DebugStack_Push(target->bin, TYPE_CSTR);
    status r = READY;
    MemCh *m = ctx->m;
    char *args[12];

    args[0] = ctx->src;
    args[1] = "/";
    args[2] = target->src;
    args[3] = NULL;
    Str *source = StrVec_Str(m, IoUtil_AbsPathBuilder(m, args));

    args[0] = ctx->dist;
    args[1] = "/bin/";
    args[2] = target->bin;
    args[3] = NULL;
    Str *dest = StrVec_Str(m, IoUtil_AbsPathBuilder(m, args));

    args[0] = ctx->dist;
    args[1] = "/bin/";
    args[2] = NULL;
    Str *binDir = StrVec_Str(m, IoUtil_AbsPathBuilder(m, args));
    Dir_CheckCreate(m, binDir);

    ProcDets pd;
    if((ctx->type.state & PROCESSING) || IoUtil_CmpUpdated(ctx->m, source, dest)){
        void *args[] = {
            source,
            dest,
            NULL
        };
        Out("^c.Building Executable & -> $^0.\n", args);

        Span *cmd = Span_Make(ctx->m);
        Span_Add(cmd, Str_CstrRef(ctx->m, ctx->tools.cc));

        char **ptr = ctx->args.cflags;
        while(*ptr != NULL){
            Span_Add(cmd, Str_CstrRef(ctx->m, *ptr));
            ptr++;
        }

        Span_Add(cmd, Str_CstrRef(ctx->m, "-o"));
        Span_Add(cmd, dest);

        ptr = ctx->args.inc;
        while(*ptr != NULL){
            Span_Add(cmd, Str_CstrRef(ctx->m, *ptr));
            ptr++;
        }

        Span_Add(cmd, source);

        if(lib != NULL){
            Span_Add(cmd, lib);
        }

        ptr = ctx->args.staticLibs;
        while(*ptr != NULL){
            Span_Add(cmd, Str_CstrRef(ctx->m, *ptr));
            ptr++;
        }

        if(ctx->args.libs != NULL){
            ptr = ctx->args.libs;
            while(*ptr != NULL){
                Span_Add(cmd, Str_CstrRef(ctx->m, *ptr));
                ptr++;
            }
        }

        ProcDets_Init(&pd);
        r |= SubProcess(ctx->m, cmd, &pd);
        if(r & ERROR){
            DebugStack_SetRef(cmd, cmd->type.of);
            void *args[] = {
                cmd,
                NULL
            };
            Fatal(FUNCNAME, FILENAME, LINENUMBER, "Build error for exec: $", args);
            return ERROR;
        }
    }
    DebugStack_Pop();
    */
    return NOOP;
}

static status buildObject(BuildCtx *ctx){
    DebugStack_Push(NULL, ZERO);
    status r = READY;
    ctx->type.state &= ~NOOP;
    char *args[5];
    MemCh *m = ctx->m;
    Span *cmd = Span_Make(m);
    ProcDets pd;

    ctx->cli.fields.current[BUILIDER_CLI_ACTION] = S(m, "build obj");
    ctx->cli.fields.current[BUILIDER_CLI_SOURCE] = ctx->current.source;
    ctx->cli.fields.current[BUILIDER_CLI_DEST] = ctx->current.dest;
    /*
    if(IoUtil_CmpUpdated(m, source, dest)){
        ctx->type.state |= PROCESSING;
        m->level--;
        LogOut(ctx);
        m->level++;

        Span_Add(cmd, S(m, ctx->tools.cc));
        char **ptr = ctx->args.cflags;
        while(*ptr != NULL){
            Span_Add(cmd, S(m, *ptr));
            ptr++;
        }
        ptr = ctx->args.inc;
        while(*ptr != NULL){
            Span_Add(cmd, S(m, *ptr));
            ptr++;
        }

        if(ctx->genConfigs != NULL){
            args[0] = ctx->dist;
            args[1] = "/";
            args[2] = ctx->libtarget;
            args[3] = "/include/";
            args[4] = NULL;
            Str *genDest = StrVec_Str(m, IoUtil_AbsPathBuilder(m, args));

            Str *s = Str_Make(m, genDest->length+2);
            Str_AddCstr(s, "-I");
            Str_Add(s, genDest->bytes, genDest->length);

            Span_Add(cmd, s);
        }

        Span_Add(cmd, Str_CstrRef(m, "-c"));
        Span_Add(cmd, Str_CstrRef(m, "-o"));
        Span_Add(cmd, dest);
        Span_Add(cmd, source);

        ProcDets_Init(&pd);
        r |= SubProcess(m, cmd, &pd);
        if(r & ERROR){
            void *args[] = {
                cmd,
                NULL
            };
            Fatal(FUNCNAME, FILENAME, LINENUMBER, "Build error for source file: $", args);
            return ERROR;
        }
    }else{
        ctx->type.state |= NOOP;
        LogOut(ctx);
    }

    Span_ReInit(cmd);
    Span_Add(cmd, Str_CstrRef(m, ctx->tools.ar));
    Span_Add(cmd, Str_CstrRef(m, "-rc"));
    Span_Add(cmd, libFileName);
    Span_Add(cmd, dest);

    ProcDets_Init(&pd);
    status re = SubProcess(m, cmd, &pd);
    if(re & ERROR){
        DebugStack_SetRef(cmd, cmd->type.of);
        Fatal(FUNCNAME, FILENAME, LINENUMBER, "Build error for adding object to lib", NULL);
        return ERROR;
    }
    */

    if(r == READY){
        r = NOOP;
    }

    DebugStack_Pop();
    return r;
}

static status buildModule(BuildCtx *ctx, Hashed *h){
    DebugStack_Push(NULL, ZERO);
    status r = READY;
    MemCh *m = ctx->m;
    void *args[5];

    StrVec *key = h->key;
    DirSelector *sel = h->value;
    /*
    args[0] = key;
    args[1] = sel;
    Out("^c.Building module @ ^0\n", args);
    */

    Str *fname = IoUtil_FnameStr(m, key);
    Str *targetName = Str_Make(m, fname->length+6);
    Str_Add(targetName, (byte *)"libcnk", 6);
    Str_Add(targetName, fname->bytes, fname->length);
    ctx->current.targetName = StrVec_From(m, targetName);

    Str *target = Str_Make(m, targetName->length+2);
    Str_Add(target, targetName->bytes, targetName->length);
    Str_Add(target, (byte *)".a", 2);

    i32 anchor = StrVec_Add(ctx->current.dest, IoUtil_PathSep(m));
    StrVec_Add(ctx->current.dest, targetName);
    StrVec_Add(ctx->current.dest, IoUtil_PathSep(m));

    Dir_CheckCreate(m, StrVec_Str(m, ctx->current.dest));
    i32 destAnchor = StrVec_Add(ctx->current.dest, target);

    ctx->cli.fields.current[BUILIDER_CLI_ACTION] = K(m, "Library build");
    ctx->cli.fields.current[BUILIDER_CLI_LIBFILENAME] = ctx->current.targetName;
    ctx->cli.fields.current[BUILIDER_CLI_SOURCE] = h->key;
    ctx->cli.fields.current[BUILIDER_CLI_DEST] = ctx->current.dest;

    microTime modified = File_ModTime(m, StrVec_Str(m, ctx->current.dest));

    ctx->input.totalModuleSources->val.value = sel->dest->nvalues;
    ctx->input.countModuleSources->val.value = 0;
    Single *skipCount = Table_Get(sel->meta, K(m, "skip-count"));
    if(skipCount != NULL){
        ctx->input.totalModuleSources->val.value -= skipCount->val.value;
    }

    ctx->type.state |= PROCESSING;

    if(modified > sel->time){
        ctx->cli.fields.current[BUILIDER_CLI_ACTION] = K(m, "Library is recent, skipping");
        LogOut(ctx);
        ctx->input.countSources->val.value += ctx->input.totalModuleSources->val.value;
        StrVec_PopTo(ctx->current.dest, anchor);
        return ZERO;
    }

    LogOut(ctx);

    i32 sourceAnchor = ctx->current.source->p->max_idx;

    Iter it;
    Iter_Init(&it, sel->dest);
    while((Iter_Next(&it) & END) == 0){
        ctx->input.countSources->val.value++;
        ctx->input.countModuleSources->val.value++;

        StrVec *v = Iter_Get(&it);
        IoUtil_Annotate(m, v);
        StrVec *source = StrVec_Make(m);
        StrVec_AddVecAfter(source, v, ctx->input.srcPrefix->p->nvalues+1);

        StrVec *object = IoUtil_SwapExt(m, source, K(m, ".c"), K(m, ".o")); 
        StrVec_AddVec(ctx->current.source, source);
        StrVec_AddVec(ctx->current.dest, object);

        ctx->cli.fields.current[BUILIDER_CLI_ACTION] = K(m, "Build object");
        ctx->cli.fields.current[BUILIDER_CLI_SOURCE] = ctx->current.source;
        ctx->cli.fields.current[BUILIDER_CLI_DEST] = ctx->current.dest;

        if(File_ModTime(m, StrVec_Str(m, ctx->current.dest)) < modified){
            ctx->cli.fields.current[BUILIDER_CLI_ACTION] = \
                K(m, "Source is recent, link only");
            LogOut(ctx);
            StrVec_PopTo(ctx->current.dest, destAnchor);
            StrVec_PopTo(ctx->current.source, sourceAnchor);
            continue;
        }

        LogOut(ctx);
        StrVec_PopTo(ctx->current.dest, destAnchor);
        StrVec_PopTo(ctx->current.source, sourceAnchor);
    }

    StrVec_PopTo(ctx->current.dest, anchor);

    /* gen */

    /* build sources */

    /*
    StrVec_Add(src, path);
    StrVec_Add(libDir, path);

    Str *dir = StrVec_Str(m, libDir);
    Dir_CheckCreate(m, dir);

    Span *p = Span_Make(m);
    Dir_Gather(m, StrVec_Str(m, src), p);

    Iter it;
    Iter_Init(&it, p);
    while((Iter_Next(&it) & END) == 0){

        StrVec *v = Iter_Get(&it);
        Str *sourceSrc = StrVec_Str(m, v);
        Str *fname = Span_Get(v->p, v->p->max_idx);

        if(v->type.state & MORE){
            StrVec_Pop(src);
            StrVec_Pop(libDir);

            Str *dirPath = Str_Clone(m, fname);
            Str_Incr(dirPath, src->total);    
            if(dirPath->bytes[dirPath->length-1] != '/'){
                Str_Add(dirPath, (byte *)"/", 1);
            }
            buildDir(ctx, dirPath, libFileName, src, libDir);
            StrVec_Add(src, path);
            StrVec_Add(libDir, path);
            continue;
        }

        StrVec_Add(libDir, fname);
        Str *dest = StrVec_Str(m, libDir);
        dest->bytes[dest->length-1] = 'o';

        ctx->fields.steps.count->val.i++;
        r |= buildObject(ctx, libFileName, dest, sourceSrc);

        StrVec_Pop(libDir);
    }

    StrVec_Pop(libDir);
    StrVec_Pop(src);
    */
    DebugStack_Pop();
    return r;
}


static status genConfigs(BuildCtx *ctx){
    /*
        GenConfig *config = ctx->genConfigs;

        args[0] = ctx->src;
        args[1] = "/gen/";
        args[2] = NULL;
        StrVec *genSrc = IoUtil_AbsPathBuilder(m, args);

        args[0] = ctx->dist;
        args[1] = "/";
        args[2] = ctx->libtarget; 
        args[3] = "/include/gen/"; 
        args[4] = NULL;
        StrVec *genDest = IoUtil_AbsPathBuilder(m, args);

        Dir_CheckCreate(m, StrVec_Str(m, genDest));

        while(config->file != NULL){

            StrVec_Add(genSrc, S(m, config->file));
            StrVec_Add(genDest, S(m, config->file));
            Str *key = S(m, config->key);

            r |= Generate(m,
                StrVec_Str(m, genSrc), key, config->args, StrVec_Str(m, genDest));
            if(r & ERROR){
                void *args[] = {
                    genSrc,
                    genDest,
                    NULL
                };
                Fatal(FUNCNAME, FILENAME, LINENUMBER, 
                    "Error generating static file: $ -> $", args);
                return ERROR;
            }

            StrVec_Pop(genSrc);
            StrVec_Pop(genDest);
            config++;
        }
    }
    */
    return ZERO;
}

static status build(BuildCtx *ctx){
    DebugStack_Push(NULL, 0);
    status r = READY;
    MemCh *m = ctx->m;
    void *args[5];

    /* build dependencies */
    Iter it;
    Iter_Init(&it, ctx->input.sources);
    while((Iter_Next(&it) & END) == 0){
        StrVec *v = StrVec_From(m, Iter_Get(&it));
        IoUtil_Annotate(m, v);
        StrVec *key = StrVec_Make(m);
        StrVec_AddVecAfter(key, v, ctx->input.srcPrefix->p->nvalues+1);
        parseDependencies(ctx, key, v);
    }

    /* gen include */
    genInclude(ctx);

    /* build libs */
    ctx->input.countModules->val.value = 0;
    StrVec_Add(ctx->current.source, IoUtil_PathSep(m));
    Iter_Init(&it, Table_Ordered(m, ctx->input.dependencies));
    while((Iter_Prev(&it) & END) == 0){
        Hashed *h = Iter_Get(&it);
        if(h != NULL){
            ctx->input.countModules->val.value = it.p->nvalues - it.idx;
            if(buildModule(ctx, h) & ERROR){
                r |= ERROR;
                break;
            }
        }
    }

    /*
    genGen(ctx);
    */

    /* build execs */

    DebugStack_Pop();
    return r;
}

status LogOut(BuildCtx *ctx){
    if(_quiet){
        void *args[7];
        if(ctx->type.state & SUCCESS){
            args[0] = ctx->cli.fields.current[BUILIDER_CLI_LIBFILENAME];
            args[1] = ctx->input.totalModules,
            args[2] = NULL;
            Out("Static Library Complete $ $ modules", args);
        }else if(ctx->type.state & ERROR){
            args[0] = ctx->cli.fields.current[BUILIDER_CLI_LIBFILENAME];
            args[1] = NULL;
            Out("Error Building Static Library $\n", args);
        }else if(ctx->type.state & PROCESSING){
            if(ctx->type.state & NOOP){
                Out("Linking $ of $ ", args);
                void *_args[] = {
                    ctx->cli.fields.current[0],
                    ctx->cli.fields.current[2],
                    NULL
                };
                Out(" for $ $ $\n", _args);
            }else{
                args[0] = ctx->input.countModuleSources;
                args[1] = ctx->input.totalModuleSources;
                args[2] = ctx->input.countModules;
                args[3] = ctx->input.totalModules;
                args[4] = ctx->input.countSources;
                args[5] = ctx->input.totalSources;
                args[6] = NULL;
                Out("Building $/$ of module $/$ total $/$", args);
                Out(" for $\n    $ $ -> $\n", ctx->cli.fields.current);
            }
        }else{
            args[0] = ctx->input.countModules;
            args[1] = ctx->input.totalModules;
            args[2] = NULL;
            Out("Module $ of $:", args);
            Out("$ $ $ -> $\n", ctx->cli.fields.current);
        }
    }else{
        if(ctx->type.state & SUCCESS){
            BuildCli_SetupComplete(ctx);
            CliStatus_Print(OutStream, ctx->cli.cli);
            CliStatus_PrintFinish(OutStream, ctx->cli.cli);
        }else if(ctx->type.state & ERROR){
            CliStatus_PrintFinish(OutStream, ctx->cli.cli);
        }else{
            CliStatus_Print(OutStream, ctx->cli.cli);
        }
    }
    return ZERO;
}

BuildCtx *BuildCtx_Make(MemCh *m){
    BuildCtx *ctx = MemCh_AllocOf(m, sizeof(BuildCtx), TYPE_BUILDCTX);
    ctx->type.of = TYPE_BUILDCTX;
    ctx->m = MemCh_Make();
    ctx->cli.cli = CliStatus_Make(m, BuildCli_RenderStatus, ctx);

    ctx->dir = StrVec_Make(m);
    ctx->src = StrVec_Make(m);

    ctx->current.target = StrVec_Make(m);
    ctx->current.targetName = StrVec_Make(m);
    ctx->current.version = StrVec_Make(m);
    ctx->current.source = StrVec_Make(m);
    ctx->current.dest = StrVec_Make(m);

    ctx->input.inc = Span_Make(m);
    ctx->input.cflags = Span_Make(m);
    ctx->input.libs = Span_Make(m);
    ctx->input.staticLibs = Span_Make(m);
    ctx->input.sources = Span_Make(m);
    ctx->input.objects = Span_Make(m);
    ctx->input.gens = Span_Make(m);
    ctx->input.dependencies = Table_Make(m);

    ctx->tools.cc = S(m, _gen_CC);
    ctx->tools.ccVersion = Str_FromI64(m, (i64)_gen_CC_VERSION);
    ctx->tools.ar = S(m, _gen_AR);

    return ctx;
}

i32 main(int argc, char **argv){
    MemBook *cp = MemBook_Make(NULL);
    void *args[5];
    if(cp == NULL){
        Fatal(FUNCNAME, FILENAME, LINENUMBER, "MemBook created successfully", NULL);
    }

    MemCh *m = MemCh_Make();
    if(m == NULL){
        Fatal(FUNCNAME, FILENAME, LINENUMBER, "MemCh created successfully", NULL);
    }

    Caneka_InitBase(m);
    BuildCtx_ToSInit(m);
    DebugStack_Push(NULL, 0);

    CliArgs *cli = CliArgs_Make(argc, argv);

    Str *helpKey = K(m, "help");
    Str *noColorKey = K(m, "no-color");
    Str *quietKey = K(m, "quiet");
    Str *runKey = K(m, "run");
    Str *modulesKey = K(m, "module");
    Str *licenceKey = K(m, "licence");
    Str *versionKey = K(m, "version");
    Str *srcKey = K(m, "src");
    Str *typeKey = K(m, "type");
    Str *dirKey = K(m, "dir");
    Str *libDirKey = K(m, "libDirs");
    Str *srcPrefixKey = K(m, "src-prefix");

    Args_Add(cli, helpKey, NULL, ARG_OPTIONAL, Sv(m, "Show this help message."));
    Args_Add(cli, noColorKey, NULL, ARG_OPTIONAL,
        Sv(m, "Skip ansi color sequences in output."));

    Args_Add(cli, srcKey, NULL, ARG_MULTIPLE,
        Sv(m, "Source code files or directories to build."));

    Args_Add(cli, srcPrefixKey, S(m, "src"), ARG_DEFAULT,
        Sv(m, "Source code files prefix. The path before the module names."));

    Span *libDirs = Span_Make(m);
    Span_Add(libDirs, S(m, "/usr/lib64"));
    Args_Add(cli, libDirKey, libDirs, ARG_MULTIPLE|ARG_DEFAULT,
        Sv(m, "lib directories to source static or linked libraries from."));

    Args_Add(cli, dirKey, S(m, "build"), ARG_DEFAULT,
        Sv(m, "Build directory to use for objects and binary assets/executables."));

    Span *types = Span_Make(m);
    Span_Add(types, S(m, "exec"));
    Span_Add(types, S(m, "static"));
    Args_Add(cli, typeKey, types, ARG_CHOICE|ARG_DEFAULT,
        Sv(m, "Type of binary asset to create, static builds a static library,"
        " exec builds and executable."));

    Args_Add(cli, quietKey, NULL, ARG_OPTIONAL,
        Sv(m, "Output a list of compiled targets instead of the progress bar."));

    Args_Add(cli, runKey, NULL, ARG_OPTIONAL,
        Sv(m, "Run the binary after it is built."));

    Args_Add(cli, licenceKey, NULL, ARG_OPTIONAL,
        Sv(m, "Show build program intellectual property licence."));
        
    Args_Add(cli, versionKey, NULL, ARG_OPTIONAL,
        Sv(m, "Show build program version."));

    CliArgs_Parse(cli);

    if(CliArgs_Get(cli, quietKey)){
        _quiet = TRUE;
    }

    BuildCtx *ctx = BuildCtx_Make(m);

    StrVec *prefix = StrVec_From(m, CliArgs_Get(cli, srcPrefixKey));
    IoUtil_Annotate(m, prefix);
    ctx->current.dest = CliArgs_GetAbsPath(cli, dirKey);
    ctx->current.source = CliArgs_GetAbsPath(cli, srcPrefixKey);
    ctx->input.sources = CliArgs_Get(cli, srcKey);
    ctx->input.srcPrefix = prefix;
    ctx->input.totalSources = I64_Wrapped(m, 0);
    ctx->input.countSources = I64_Wrapped(m, 0);
    ctx->input.totalModules = I64_Wrapped(m, 0);
    ctx->input.countModules = I64_Wrapped(m, 0);
    ctx->input.totalModuleSources = I64_Wrapped(m, 0);
    ctx->input.countModuleSources = I64_Wrapped(m, 0);

    build(ctx);

    args[0] = cli->args;
    args[1] = ctx;
    args[2] = NULL;
    Out("^p.Args @\nCtx &^0\n", args);

    CliArgs_Free(cli);

    DebugStack_Pop();
    return 0;
}
