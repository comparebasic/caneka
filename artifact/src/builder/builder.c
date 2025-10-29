#include <external.h>
#include <caneka.h>
#include <builder.h>

static status renderStatus(MemCh *m, Abstract *a){
    CliStatus *cli = (CliStatus *)as(a, TYPE_CLI_STATUS);
    BuildCtx *ctx = (BuildCtx *)as(cli->source, TYPE_BUILDCTX);

    i32 width = ctx->cli->cols;
    float total = (float)ctx->fields.steps.total->val.i;
    float count = (float)ctx->fields.steps.count->val.i;
    float colsFloat = (float)width;

    float progress = ceil((count/total) * colsFloat);
    ctx->fields.steps.barStart->length = (i32)progress;

    float modSrcTotal = count + (float)(ctx->fields.steps.modSrcTotal->val.i-ctx->fields.steps.modSrcCount->val.i);
    ctx->fields.steps.barEnd->length = (i32)ceil(((modSrcTotal)/total) * colsFloat) - progress;

    CliStatus_SetByKey(m, cli, Str_CstrRef(m, "memTotal"),
        (Abstract *)Str_MemCount(ctx->m, MemChapterTotal()*PAGE_SIZE));
    CliStatus_SetByKey(m, cli, Str_CstrRef(m, "memUsed"),
        (Abstract *)Str_MemCount(ctx->m, MemCount(0)));
    Single *sg = (Single *)as(CliStatus_GetByKey(m, 
        cli, Str_CstrRef(m, "chapters")), TYPE_WRAPPED_I64);
    sg->val.value = MemChapterCount();
    sg = (Single *)as(CliStatus_GetByKey(m, 
        cli, Str_CstrRef(m, "chaptersTotal")), TYPE_WRAPPED_I64);
    sg->val.value = MemChapterTotal();

    return SUCCESS;
}

static status setupComplete(BuildCtx *ctx){
    FmtLine *ln = Span_Get(ctx->cli->lines, 0);
    ln->fmt = "^g.Complete - ^D.$^d.sources/^D.$^d.modules^0.";
    ln->args = Arr_Make(ctx->m, 2);
    ln->args[0] = (Abstract *)ctx->fields.steps.total;
    ln->args[1] = (Abstract *)ctx->fields.steps.modTotal;

    ln = Span_Get(ctx->cli->lines, 1);
    ln->fmt = "";
    ln = Span_Get(ctx->cli->lines, 2);
    ln->fmt = "";

    ln = Span_Get(ctx->cli->lines, 3);
    ctx->fields.steps.barStart->length = ctx->cli->cols;
    ln->fmt = "^G.$^0.";

    ln = Span_Get(ctx->cli->lines, 4);
    ln->args[0] = (Abstract *)Str_Ref(ctx->m, (byte *)"g.", 2, 3, STRING_FMT_ANSI);

    return SUCCESS;
}

static status setupStatus(BuildCtx *ctx){
    Span *lines = ctx->cli->lines;
    MemCh *m = ctx->m;
    memset(&ctx->fields, 0, sizeof(ctx->fields));
    ctx->fields.steps.total = I32_Wrapped(m, 0);
    ctx->fields.steps.count = I32_Wrapped(m, 0);
    ctx->fields.steps.modSrcCount = I32_Wrapped(m, 0);
    ctx->fields.steps.modSrcTotal = I32_Wrapped(m, 0);
    ctx->fields.steps.modCount = I32_Wrapped(m, 0);
    ctx->fields.steps.modTotal = I32_Wrapped(m, 0);
    ctx->fields.steps.name = Str_Make(m, STR_DEFAULT);
    ctx->fields.current.source = Str_Make(m, STR_DEFAULT);
    ctx->fields.current.dest = Str_Make(m, STR_DEFAULT);
    ctx->fields.current.action = Str_Make(m, STR_DEFAULT);

    BuildSubdir **dir = ctx->objdirs;
    while(*dir != NULL){
        char **sourceCstr = (*dir)->sources;
        while(*sourceCstr != NULL){
            ctx->fields.steps.total->val.i++;
            sourceCstr++;
        }
        dir++;
    }

    dir = ctx->objdirs;
    while(*dir != NULL){
        ctx->fields.steps.modTotal->val.i++;
        dir++;
    }

    if(ctx->args.licenceFiles != NULL){
        ctx->fields.steps.modTotal->val.i++;
    }

    CliStatus_SetDims(ctx->cli, 0, 0);
    i32 width = ctx->cli->cols;
    IntPair coords = {0, 0};

    Abstract **arr = Arr_Make(m, 3);
    arr[0] = (Abstract *)ctx->fields.steps.modCount;
    arr[1] = (Abstract *)ctx->fields.steps.modTotal;
    arr[2] = (Abstract *)ctx->fields.steps.name;
    Span_Add(ctx->cli->lines, 
        (Abstract *)FmtLine_Make(m, "^y.module $ of $: ^D.$^0", arr));

    arr = Arr_Make(m, 3);
    arr[0] = (Abstract *)ctx->fields.current.action;
    arr[1] = (Abstract *)ctx->fields.current.source;

    Span_Add(ctx->cli->lines, 
        (Abstract *)FmtLine_Make(m, "^b.$: $", arr));

    arr = Arr_Make(m, 1);
    arr[0] = (Abstract *)ctx->fields.current.dest;
    Span_Add(ctx->cli->lines, 
        (Abstract *)FmtLine_Make(m, "    -> ^D.$^0", arr));

    ctx->fields.steps.barStart = Str_Make(m, width);
    memset(ctx->fields.steps.barStart->bytes, ' ', width);
    ctx->fields.steps.barStart->length = width;
    ctx->fields.steps.barEnd = Str_CloneAlloc(m, ctx->fields.steps.barStart, width);
    ctx->fields.steps.barStart->length = 0;
    
    arr = Arr_Make(m, 2);
    arr[0] = (Abstract *)ctx->fields.steps.barStart; 
    arr[1] = (Abstract *)ctx->fields.steps.barEnd;
    Span_Add(ctx->cli->lines,
        (Abstract *)FmtLine_Make(m, "^B.$^0.^Y.$^0", arr));

    arr = Arr_Make(m, 5);
    arr[0] = (Abstract *)Str_Ref(m, (byte *)"c.", 2, 3, STRING_FMT_ANSI);
    arr[1] = NULL;
    arr[2] = (Abstract *)I64_Wrapped(m, 0);
    arr[3] = (Abstract *)I64_Wrapped(m, 0);
    arr[4] = NULL;
    Span_Add(ctx->cli->lines,
        (Abstract *)FmtLine_Make(m,
            "$Memory $ ($ of $ chapters) ^D.$^d.used^0", arr));

    coords.a = ctx->cli->lines->max_idx;
    coords.b = 1;
    CliStatus_SetKey(m, ctx->cli, Str_CstrRef(m, "memTotal"), &coords);
    coords.b = 2;
    CliStatus_SetKey(m, ctx->cli, Str_CstrRef(m, "chapters"), &coords);
    coords.b = 3;
    CliStatus_SetKey(m, ctx->cli, Str_CstrRef(m, "chaptersTotal"), &coords);
    coords.b = 4;
    CliStatus_SetKey(m, ctx->cli, Str_CstrRef(m, "memUsed"), &coords);

    return SUCCESS;
}

static status buildExec(BuildCtx *ctx, boolean force, Str *destDir, Str *lib, Executable *target){
    DebugStack_Push(target->bin, TYPE_CSTR);
    status r = READY;
    Str *source = Str_Make(ctx->m, STR_DEFAULT);
    Str_AddCstr(source, ctx->src);
    Str_AddCstr(source, "/");
    Str_AddCstr(source, target->src);

    Str *dest = Str_Make(ctx->m, STR_DEFAULT);
    Str_AddCstr(dest, ctx->dist);
    Str_AddCstr(dest, "/bin/");
    Dir_CheckCreate(ctx->m, dest);
    Str_AddCstr(dest, target->bin);

    ProcDets pd;
    if(IoUtil_CmpUpdated(ctx->m, source, dest, NULL)){
        Abstract *args[] = {
            (Abstract *)source,
            (Abstract *)dest,
            NULL
        };
        Out("^c.Building Executable $ -> $^0.\n", args);

        Span *cmd = Span_Make(ctx->m);
        Span_Add(cmd, (Abstract *)Str_CstrRef(ctx->m, ctx->tools.cc));

        char **ptr = ctx->args.cflags;
        while(*ptr != NULL){
            Span_Add(cmd, (Abstract *)Str_CstrRef(ctx->m, *ptr));
            ptr++;
        }

        Span_Add(cmd, (Abstract *)Str_CstrRef(ctx->m, "-o"));
        Span_Add(cmd, (Abstract *)dest);

        ptr = ctx->args.inc;
        while(*ptr != NULL){
            Span_Add(cmd, (Abstract *)Str_CstrRef(ctx->m, *ptr));
            ptr++;
        }

        Span_Add(cmd, (Abstract *)source);

        if(lib != NULL){
            Span_Add(cmd, (Abstract *)lib);
        }
        ptr = ctx->args.staticLibs;
        while(*ptr != NULL){
            Span_Add(cmd, (Abstract *)Str_CstrRef(ctx->m, *ptr));
            ptr++;
        }

        ptr = ctx->args.libs;
        while(*ptr != NULL){
            Span_Add(cmd, (Abstract *)Str_CstrRef(ctx->m, *ptr));
            ptr++;
        }


        ProcDets_Init(&pd);
        r |= SubProcess(ctx->m, cmd, &pd);
        if(r & ERROR){
            DebugStack_SetRef(cmd, cmd->type.of);
            Abstract *args[] = {
                (Abstract *)cmd,
                NULL
            };
            Fatal(FUNCNAME, FILENAME, LINENUMBER, "Build error for exec: $", args);
            return ERROR;
        }
    }
    DebugStack_Pop();
    return NOOP;
}

static status buildSourceToLib(BuildCtx *ctx, Str *libDir, Str *lib,Str *dest, Str *source){
    DebugStack_Push(source, source->type.of);
    status r = READY;
    MemCh *m = ctx->m;
    Span *cmd = Span_Make(m);
    ProcDets pd;

    if(IoUtil_CmpUpdated(m, source, dest, NULL)){
        Str_Reset(ctx->fields.current.action);
        Str_AddCstr(ctx->fields.current.action, "build obj");
        CliStatus_Print(OutStream, ctx->cli);

        Span_Add(cmd, (Abstract *)Str_CstrRef(m, ctx->tools.cc));
        char **ptr = ctx->args.cflags;
        while(*ptr != NULL){
            Span_Add(cmd, (Abstract *)Str_CstrRef(m, *ptr));
            ptr++;
        }
        ptr = ctx->args.inc;
        while(*ptr != NULL){
            Span_Add(cmd, (Abstract *)Str_CstrRef(m, *ptr));
            ptr++;
        }

        if(ctx->genConfigs != NULL){
            Str *s = Str_Make(m, STR_DEFAULT);
            Str_AddCstr(s, ctx->dist);
            Str_AddCstr(s, "/");
            Str_AddCstr(s, ctx->libtarget);
            Str_AddCstr(s, "/include/");
            Str *genDest = IoUtil_GetAbsPath(m, s);

            s = Str_Make(m, STR_DEFAULT);
            Str_AddCstr(s, "-I");
            Str_Add(s, genDest->bytes, genDest->length);

            Span_Add(cmd, (Abstract *)s);
        }

        Span_Add(cmd, (Abstract *)Str_CstrRef(m, "-c"));
        Span_Add(cmd, (Abstract *)Str_CstrRef(m, "-o"));
        Span_Add(cmd, (Abstract *)dest);
        Span_Add(cmd, (Abstract *)source);

        ProcDets_Init(&pd);
        r |= SubProcess(m, cmd, &pd);
        if(r & ERROR){
            Abstract *args[] = {
                (Abstract *)cmd,
                NULL
            };
            Fatal(FUNCNAME, FILENAME, LINENUMBER, "Build error for source file: $", args);
            return ERROR;
        }

        Span_ReInit(cmd);
        Span_Add(cmd, (Abstract *)Str_CstrRef(m, ctx->tools.ar));
        Span_Add(cmd, (Abstract *)Str_CstrRef(m, "-rc"));
        Span_Add(cmd, (Abstract *)lib);
        Span_Add(cmd, (Abstract *)dest);
        ProcDets_Init(&pd);
        status re = SubProcess(m, cmd, &pd);
        if(re & ERROR){
            DebugStack_SetRef(cmd, cmd->type.of);
            Fatal(FUNCNAME, FILENAME, LINENUMBER, "Build error for adding object to lib", NULL);
            return ERROR;
        }
    }else{
        Str_Reset(ctx->fields.current.action);
        Str_AddCstr(ctx->fields.current.action, " link obj");
        CliStatus_Print(OutStream, ctx->cli);
    }

    if(r == READY){
        r = NOOP;
    }

    DebugStack_Pop();
    return r;
}

static status buildDirToLib(BuildCtx *ctx, Str *libDir, Str *lib, BuildSubdir *dir){
    DebugStack_Push(NULL, 0);
    status r = READY;
    MemCh *m = ctx->m;

    Str *dirPath = Str_CloneAlloc(m, libDir, STR_DEFAULT);
    Str_AddCstr(dirPath, "/");
    Str_AddCstr(dirPath, dir->name);
    DebugStack_SetRef(dirPath, dirPath->type.of);
    Dir_CheckCreate(m, dirPath);

    Str *source = ctx->fields.current.source;
    Str_Reset(source);
    Str *_source = IoUtil_GetAbsPath(m, Str_CstrRef(m, ctx->src));
    Str_Add(source, _source->bytes, _source->length);
    Str_AddCstr(source, "/");
    Str_AddCstr(source, dir->name);
    Str_AddCstr(source, "/");
    i64 sourceL = source->length;

    Str *dest = ctx->fields.current.dest;
    Str_Reset(dest);
    Str_Add(dest, dirPath->bytes, dirPath->length);
    Str_AddCstr(dest, "/");
    i64 destL = dest->length;

    m->level++;

    ctx->fields.steps.modSrcCount->val.i = 0;
    ctx->fields.steps.modSrcTotal->val.i = 0;
    char **sourceCstr = dir->sources;
    while(*sourceCstr != NULL){
        ctx->fields.steps.modSrcTotal->val.i++;
        sourceCstr++;
    }

    Str_Reset(ctx->fields.steps.name);
    Str_AddCstr(ctx->fields.steps.name, dir->name);

    sourceCstr = dir->sources;
    ctx->fields.steps.modCount->val.i++;
    while(*sourceCstr != NULL){
        m->level++;
        Str_Trunc(source, sourceL);
        Str_AddCstr(source, *sourceCstr);
        Str_Trunc(dest, destL);
        Str_AddCstr(dest, *sourceCstr);
        Str_Trunc(dest, -1);
        Str_AddCstr(dest, "o");
        r |= buildSourceToLib(ctx, libDir, lib, dest, source);
        ctx->fields.steps.count->val.i++;
        ctx->fields.steps.modSrcCount->val.i++;

        MemCh_Free(m);
        m->level--;
        sourceCstr++;
        if(r & ERROR){
            return r;
        }
    }
    ctx->fields.steps.modSrcCount->val.i = 0;
    ctx->fields.steps.modSrcTotal->val.i = 0;
    CliStatus_Print(OutStream, ctx->cli);
    m->level--;

    DebugStack_Pop();
    return r;
}

static status build(BuildCtx *ctx){
    status r = READY;
    DebugStack_Push(NULL, 0);
    Abstract *args[5];
    MemCh *m = ctx->m;
    setupStatus(ctx);
    Str *libDir = IoUtil_GetAbsPath(m, Str_CstrRef(m, ctx->dist));

    Str_AddCstr(libDir, "/");
    Str_AddCstr(libDir, ctx->libtarget);

    Dir_CheckCreate(m, libDir);

    Str *lib = Str_CloneAlloc(m, libDir, STR_DEFAULT);
    Str_AddCstr(lib, "/");
    Str_AddCstr(lib, ctx->libtarget);
    Str_AddCstr(lib, ".a");
    if(lib->type.state & ERROR){
        return ERROR;
    }

    if(ctx->genConfigs != NULL){
        GenConfig *config = ctx->genConfigs;
        i32 len = strlen(ctx->src);
        Str *s = Str_Make(m, STR_DEFAULT);
        Str_AddCstr(s, ctx->src);
        Str_AddCstr(s, "/gen/");
        Str *genSrc = IoUtil_GetAbsPath(m, s);

        s = Str_Make(m, STR_DEFAULT);
        Str_AddCstr(s, ctx->dist);
        Str_AddCstr(s, "/");
        Str_AddCstr(s, ctx->libtarget);
        Str_AddCstr(s, "/include/gen/");
        Str *genDest = IoUtil_GetAbsPath(m, s);

        Dir_CheckCreate(m, genDest);

        i16 genSrcL = genSrc->length;
        i16 genDestL = genDest->length;
        while(config->file != NULL){

            Str_AddCstr(genSrc, config->file);
            Str_AddCstr(genDest, config->file);
            Str *key = Str_CstrRef(m, config->key);

            if((genSrc->type.state|genSrc->type.state) & ERROR){
                return ERROR;
            }

            r |= Generate(m, genSrc, key, config->args, genDest);
            if(r & ERROR){
                args[0] = (Abstract *)genSrc;
                args[1] = (Abstract *)genDest;
                args[2] = NULL;
                Fatal(FUNCNAME, FILENAME, LINENUMBER, "Error generating static file: $ -> $", args);
                return ERROR;
            }

            Str_Trunc(genSrc, genSrcL);
            Str_Trunc(genDest, genDestL);
            config++;
        }
    }

    BuildSubdir **dir = ctx->objdirs;
    while(*dir != NULL){
        r |= buildDirToLib(ctx, libDir, lib, *dir);
        dir++;
    }
    if((r & ERROR) == 0){
        setupComplete(ctx);
    }else{
        return r;
    }
    CliStatus_Print(OutStream, ctx->cli);
    CliStatus_PrintFinish(OutStream, ctx->cli);

    Str *dist = IoUtil_GetAbsPath(m, Str_CstrRef(m, ctx->dist));
    Executable *target = ctx->targets;
    while(target->bin != NULL){
        buildExec(ctx, ((r & SUCCESS) != 0), dist, lib, target);
        target++;
    }
    DebugStack_Pop();
    return r;
}

status Init(MemCh *m){
    status r = READY;
    r |= Debug_Init(m);
    r |= DebugStack_Init(m);
    return r;
}

status BuildCtx_Init(MemCh *m, BuildCtx *ctx){
    Init(m);
    memset(ctx, 0, sizeof(BuildCtx));
    ctx->type.of = TYPE_BUILDCTX;
    ctx->m = MemCh_Make();
    ctx->cli = CliStatus_Make(m, renderStatus, (Abstract *)ctx);
    return SUCCESS;
}

status Build(BuildCtx *ctx){
    DebugStack_Push(ctx, ctx->type.of);
    status r = READY;
    r |= build(ctx);
    DebugStack_Pop();
    return r;
}
