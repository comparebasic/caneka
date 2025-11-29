#include <external.h>
#include <caneka.h>
#include "module.h"

static boolean _quiet = FALSE;

static status parseDependencies(BuildCtx *ctx, StrVec *path){
    void *args[5];
    MemCh *m = ctx->m;
    i32 idx = StrVec_AddVecAfter(   
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
        Table_Set(ctx->input.dependencies, path, sel);
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

    StrVec_PopTo(ctx->current.source, idx);

    Str *shelf = Str_Make(m, STR_DEFAULT);
    Cursor *curs = Cursor_Make(m, bf->v);
    Str *label = NULL;
    while((Cursor_NextByte(curs) & END) == 0){
        if(*curs->ptr == '\n'){
            if(label != NULL && label->length > 0){
                if(Equals(label, K(m, "exec"))){
                    shelf->type.state |= BUILD_EXEC; 
                }else if(Equals(label, K(m, "static"))){
                    shelf->type.state |= BUILD_STATIC; 
                }else if(Equals(label, K(m, "link"))){
                    shelf->type.state |= BUILD_LINK; 
                }else if(Equals(label, K(m, "skip"))){
                    shelf->type.state |= BUILD_SKIP; 
                }
                Span_Add(sel->exclude, shelf);
                label = NULL;
                shelf = Str_Make(m, STR_DEFAULT);
                continue;
            }
            StrVec *v = StrVec_From(m, shelf);
            IoUtil_Annotate(ctx->m, v);

            path = StrVec_Copy(m, ctx->input.srcPrefix);
            StrVec_Add(path, IoUtil_PathSep(m));
            StrVec_Add(path, shelf);

            parseDependencies(ctx, path);
            shelf = Str_Make(m, STR_DEFAULT);
        }else if(*curs->ptr == '='){
            label = shelf;
            shelf = Str_Make(m, STR_DEFAULT);
        }else{
            Str_Add(shelf, curs->ptr, 1);
        }
    }

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

static status buildObject(BuildCtx *ctx,
        Str *libFileName,
        Str *dest,
        Str *source
    ){
    status r = READY;
    /*
    ctx->type.state &= ~NOOP;
    DebugStack_Push(source, source->type.of);
    char *args[5];
    MemCh *m = ctx->m;
    Span *cmd = Span_Make(m);
    ProcDets pd;

    ctx->fields.current[BUILIDER_CLI_ACTION] = S(m, "build obj");
    ctx->fields.current[BUILIDER_CLI_SOURCE] = source;
    ctx->fields.current[BUILIDER_CLI_DEST] = dest;
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

    if(r == READY){
        r = NOOP;
    }

    DebugStack_Pop();
    */
    return r;
}

static status buildDir(BuildCtx *ctx,
        Str *path, Str *libFileName, StrVec *src, StrVec *libDir){

    status r = READY;
    MemCh *m = ctx->m;
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
    return r;
}

static status gatherTotal(BuildCtx *ctx, Str *source){
    status r = READY;
    MemCh *m = ctx->m;
    /*

    if(source->bytes[source->length-1] == '/'){
        Span *p = Span_Make(m);
        Dir_Gather(m, source, p);
        Iter it;
        Iter_Init(&it, p);
        while((Iter_Next(&it) & END) == 0){
            StrVec *v = Iter_Get(&it);
            if(v->type.state & MORE){
                Span *p = Span_Make(m);
                Dir_Gather(m, StrVec_Str(m, v), p);
                Iter _it;
                Iter_Init(&_it, p);
                while((Iter_Next(&_it) & END) == 0){
                    gatherTotal(ctx, StrVec_Str(m, Iter_Get(&_it)));
                }
            }else{
                ctx->fields.steps.total->val.i++;
            }
        }
    }else{
        ctx->fields.steps.total->val.i++;
    }
    */

    return ZERO;
}

static status build(BuildCtx *ctx){
    status r = READY;
    DebugStack_Push(NULL, 0);
    /*
    char *args[5];
    void *out[4];
    MemCh *m = ctx->m;

    BuildCli_SetupStatus(ctx);

    args[0] = ctx->src;
    args[1] = "/";
    args[2] = NULL;
    StrVec *src = IoUtil_AbsPathBuilder(m, args);

    Str *lib = S(m, ctx->libtarget);

    args[0] = ctx->dist;
    args[1] = "/";
    args[2] = ctx->libtarget;
    args[3] = "/";
    args[4] = NULL;
    StrVec *libDir = IoUtil_AbsPathBuilder(m, args);

    Str *libPath = StrVec_Str(m, libDir);
    Dir_CheckCreate(m, libPath);

    Str *targetName = Str_Make(m, strlen(ctx->libtarget)+3);
    Str_AddCstr(targetName, ctx->libtarget);
    Str_AddCstr(targetName, ".a");
    StrVec_Add(libDir, targetName);
    Str *libFileName = StrVec_Str(m, libDir);
    StrVec_Pop(libDir);
    ctx->fields.current[BUILIDER_CLI_LIBFILENAME] = targetName;

    File_Unlink(m, libFileName);

    if(ctx->genConfigs != NULL){
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
    
    ctx->fields.steps.total->val.i = 0;
    char **sourcePtr = ctx->sources;
    while(sourcePtr != NULL && *sourcePtr != NULL){
        StrVec_Add(src, S(m, *sourcePtr));
        gatherTotal(ctx, StrVec_Str(m, src));
        StrVec_Pop(src);
        sourcePtr++;
    }

    i32 libCount = 0;
    sourcePtr = ctx->sources;
    while(sourcePtr != NULL && *sourcePtr != NULL){
        m->level++;
        Str *source = S(m, *sourcePtr);
        if(source->bytes[source->length-1] == '/'){
            r |= buildDir(ctx, source, libFileName, src, libDir);
        }else{
            Str *s = source;
            StrVec_Add(libDir, s);
            StrVec_Add(src, s);

            Str *sourceSrc = StrVec_Str(m, src);
            Str *dest = StrVec_Str(m, libDir);
            dest->bytes[dest->length-1] = 'o';
            ctx->fields.steps.count->val.i++;
            r |= buildObject(ctx, libFileName, dest, sourceSrc);

            StrVec_Pop(libDir);
            StrVec_Pop(src);
        }

        sourcePtr++;
        libCount++;
        m->level--;
        MemCh_FreeTemp(m);
    }

    if((r & ERROR) == 0){
        ctx->type.state |= SUCCESS;
    }else{
        ctx->type.state |= ERROR;
        return r;
    }

    LogOut(ctx);

    Str *dist = IoUtil_GetAbsPath(m, Str_CstrRef(m, ctx->dist));
    if(ctx->targets != NULL){
        Executable *target = ctx->targets;
        while(target->bin != NULL){
            if(libCount == 0){
                lib = NULL;
            }
            / *
             * ToDo: detect any updated lib to only rebuild the binary
             * If a child library has been updated
             * /
            ctx->type.state |= PROCESSING;
            buildExec(ctx, ((r & SUCCESS) != 0), dist, libFileName, target);
            target++;
        }
    }
    */
    DebugStack_Pop();
    return r;
}


status LogOut(BuildCtx *ctx){
    /*
    if(_quiet){
        void *args[3];
        args[0] = ctx->fields.steps.count,
        args[1] = ctx->fields.steps.total,
        args[2] = NULL;

        if(ctx->type.state & SUCCESS){
            args[0] = ctx->fields.current[BUILIDER_CLI_LIBFILENAME];
            args[1] = NULL;
            Out("Static Library Complete $\n", args);
        }else if(ctx->type.state & ERROR){
            args[0] = ctx->fields.current[BUILIDER_CLI_LIBFILENAME];
            args[1] = NULL;
            args[1] = NULL;
            Out("Error Building Static Library $\n", args);
        }else{
            if(ctx->type.state & NOOP){
                Out("Linking $ of $ ", args);
                void *_args[] = {ctx->fields.current[0], ctx->fields.current[2], NULL};
                Out("for $ $ $\n", _args);
            }else{
                Out("Building $ of $ ", args);
                Out("for $ $ $ -> $\n", ctx->fields.current);
            }
        }
    }else{
        if(ctx->type.state & SUCCESS){
            BuildCli_SetupComplete(ctx);
            CliStatus_Print(OutStream, ctx->cli);
            CliStatus_PrintFinish(OutStream, ctx->cli);
        }else if(ctx->type.state & ERROR){
            CliStatus_PrintFinish(OutStream, ctx->cli);
        }else{
            CliStatus_Print(OutStream, ctx->cli);
        }
    }
    */
    return ZERO;
}

status Build(BuildCtx *ctx, i32 argc, char *argv[]){
    DebugStack_Push(ctx, ctx->type.of);
    status r = READY;
    /*
    if(argc >= 2 && Equals(K(ctx->m, argv[1]), K(ctx->m, "--quiet"))){
        _quiet = TRUE;
    }
    status r = READY;
    r |= build(ctx);
    */
    DebugStack_Pop();
    return r;
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
    Str *srcPrefixKey = K(m, "src-prefix");

    Args_Add(cli, helpKey, NULL, ARG_OPTIONAL, Sv(m, "Show this help message."));
    Args_Add(cli, noColorKey, NULL, ARG_OPTIONAL,
        Sv(m, "Skip ansi color sequences in output."));
    Args_Add(cli, srcKey, NULL, ARG_MULTIPLE,
        Sv(m, "Source code files or directories to build."));
    Args_Add(cli, srcPrefixKey, S(m, "src"), ARG_DEFAULT,
        Sv(m, "Source code files prefix. The path before the module names."));
    Args_Add(cli, dirKey, S(m, "./build"), ARG_DEFAULT,
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

    Iter it;
    BuildCtx *ctx = BuildCtx_Make(m);

    StrVec *prefix = StrVec_From(m, CliArgs_Get(cli, srcPrefixKey));
    IoUtil_Annotate(m, prefix);
    ctx->input.srcPrefix = prefix;
    ctx->current.source = CliArgs_GetAbsPath(cli, srcPrefixKey);
    ctx->input.sources = CliArgs_Get(cli, srcKey);

    Iter_Init(&it, ctx->input.sources);
    while((Iter_Next(&it) & END) == 0){
        StrVec *v = StrVec_From(m, Iter_Get(&it));
        IoUtil_Annotate(m, v);
        parseDependencies(ctx, v);
    }

    args[0] = cli->args;
    args[1] = ctx;
    args[2] = NULL;
    Out("^p.Args @\nCtx &^0\n", args);

    CliArgs_Free(cli);

    DebugStack_Pop();
    return 0;
}


