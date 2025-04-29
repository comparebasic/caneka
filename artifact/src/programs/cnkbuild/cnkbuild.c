#include <external.h>
#include <caneka.h>
#include <cnkbuild.h>

static status renderStatus(MemCh *m, Abstract *a){
    CliStatus *cli = (CliStatus *)as(a, TYPE_CLI_STATUS);
    BuildCtx *ctx = (BuildCtx *)as(cli->source, TYPE_BUILDCTX);

    i32 width = ctx->cli->cols;
    float _progress = ((float)ctx->cli->cols) * ((float)ctx->fields.steps.count->val.i)/((float)ctx->fields.steps.total->val.i);
    i32 progress = (i32)_progress;
    ctx->fields.steps.barStart->length = progress;

    i32 remainingModSrc = ctx->fields.steps.modSrcTotal->val.i - ctx->fields.steps.modSrcCount->val.i;
    _progress = ((float)ctx->cli->cols) * ((float)remainingModSrc)/((float)ctx->fields.steps.total->val.i);
    progress = (i32)_progress;
    ctx->fields.steps.barEnd->length = progress;

    Single *sg = (Single *)as(CliStatus_GetByKey(m, cli, Str_CstrRef(m, "memCount")), TYPE_WRAPPED_I64);
    sg->val.value = MemCount(0);
     
    return SUCCESS;
}

static status setupStatus(BuildCtx *ctx){
    Span *lines = ctx->cli->lines;
    MemCh *m = ctx->m;
    memset(&ctx->fields, 0, sizeof(ctx->fields));
    ctx->fields.steps.total = I32_Wrapped(m, 0);
    ctx->fields.steps.name = Str_Make(m, STR_DEFAULT);

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
    ctx->fields.steps.modCount = I32_Wrapped(m, 0);
    ctx->fields.steps.modTotal = I32_Wrapped(m, 0);
    while(*dir != NULL){
        ctx->fields.steps.modTotal->val.i++;
        dir++;
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
    arr[2] = (Abstract *)ctx->fields.current.dest;

    Span_Add(ctx->cli->lines, 
        (Abstract *)FmtLine_Make(m, "^b.$: $ -> ^D.$^0", arr));

    ctx->fields.steps.barStart = Str_Make(m, width);
    memset(ctx->fields.steps.barStart->bytes, ' ', width);
    ctx->fields.steps.barEnd = Str_Clone(m, ctx->fields.steps.barStart, width);
    
    arr = Arr_Make(m, 2);
    arr[0] = (Abstract *)ctx->fields.steps.barStart; 
    arr[1] = (Abstract *)ctx->fields.steps.barEnd;
    Span_Add(ctx->cli->lines,
        (Abstract *)FmtLine_Make(m, "^B.$^0.^Y.$^0", arr));

    arr = Arr_Make(m, 1);
    arr[0] = (Abstract *)MemCount_Wrapped(m, 0);
    Span_Add(ctx->cli->lines,
        (Abstract *)FmtLine_Make(m, "^c.Memory Count: $^0", arr));

    coords.a = ctx->cli->lines->max_idx;
    coords.b = 0;
    CliStatus_SetKey(m, ctx->cli, Str_CstrRef(m, "memCount"), &coords);

    return SUCCESS;
}

static status buildExec(BuildCtx *ctx, boolean force, Str *destDir, Str *lib, Executable *target){
    DebugStack_Push(target->bin, TYPE_CSTR);
    status r = READY;
    /*
    MemCh *m = ctx->m;
    Span *cmd = Span_Make(m);
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
    Span_Add(cmd, (Abstract *)Str_CstrRef(m, "-o"));

    Str *dest = Str_Make(m, STR_DEFAULT);
    Str_Add(dest, destDir->bytes, destDir->length);
    Str_AddCstr(dest, "/");
    Str_AddCstr(dest, target->bin);
    if(dest->type.state & ERROR){
        Fatal(0, FUNCNAME, FILENAME, LINENUMBER, "Error building dest str");
    }
    Span_Add(cmd, (Abstract *)dest);

    Str *source = File_GetAbsPath(m, Str_CstrRef(m, ctx->src));
    Str_AddCstr(source, "/programs/");
    Str_AddCstr(source, target->src);

    Span_Add(cmd, (Abstract *)source);
    Span_Add(cmd, (Abstract *)lib);


    ptr = ctx->args.libs;
    while(*ptr != NULL){
        Span_Add(cmd, (Abstract *)Str_CstrRef(m, *ptr));
        ptr++;
    }

    if(force || File_CmpUpdated(m, source, dest, NULL)){
        Out("^cbuild exec: _t^0\n", dest);

        DebugStack_SetRef(cmd, cmd->type.of);
        ProcDets pd;
        ProcDets_Init(&pd);
        r |= SubProcess(m, cmd, &pd);
        if(r & ERROR){
            Fatal(0, FUNCNAME, FILENAME, LINENUMBER, "Build error for source file");
        }

        DebugStack_Pop();
        return r;
    }
    */
    DebugStack_Pop();
    return NOOP;
}

static status buildSourceToLib(BuildCtx *ctx, Str *libDir, Str *lib,Str *dest, Str *source){
    DebugStack_Push(source, source->type.of);
    status r = READY;
    MemCh *m = ctx->m;
    Span *cmd = Span_Make(m);
    ProcDets pd;

    ctx->fields.steps.count->val.i++;
    ctx->fields.steps.modSrcCount->val.i++;
    if(File_CmpUpdated(m, source, dest, NULL)){
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
        Span_Add(cmd, (Abstract *)Str_CstrRef(m, "-c"));
        Span_Add(cmd, (Abstract *)Str_CstrRef(m, "-o"));
        Span_Add(cmd, (Abstract *)dest);
        Span_Add(cmd, (Abstract *)source);

        ProcDets_Init(&pd);
        r |= SubProcess(m, cmd, &pd);
        if(r & ERROR){
            DebugStack_SetRef(cmd, cmd->type.of);
            Fatal(FUNCNAME, FILENAME, LINENUMBER, "Build error for source file", NULL);
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

    Str *dirPath = Str_Clone(m, libDir, STR_DEFAULT);
    Str_AddCstr(dirPath, "/");
    Str_AddCstr(dirPath, dir->name);
    DebugStack_SetRef(dirPath, dirPath->type.of);
    Dir_CheckCreate(m, dirPath);

    Str *source = ctx->fields.current.source;
    Str_Reset(source);
    Str *_source = File_GetAbsPath(m, Str_CstrRef(m, ctx->src));
    Str_Add(source, _source->bytes, source->length);
    Str_AddCstr(source, "/");
    Str_AddCstr(source, dir->name);
    Str_AddCstr(source, "/");
    i64 sourceL = source->length;

    Str *dest = ctx->fields.current.dest;
    Str_Reset(dest);
    Str_Add(dest, dirPath->bytes, dirPath->length);
    Str_AddCstr(dest, "/");
    i64 destL = dest->length;

    m->type.range++;

    ctx->fields.steps.modSrcCount = 0;
    ctx->fields.steps.modSrcTotal = 0;
    char **sourceCstr = dir->sources;
    while(*sourceCstr != NULL){
        ctx->fields.steps.modSrcTotal++;
        sourceCstr++;
    }

    sourceCstr = dir->sources;

    while(*sourceCstr != NULL){
        Str_Trunc(source, sourceL);
        Str_AddCstr(source, *sourceCstr);
        Str_Trunc(dest, destL);
        Str_AddCstr(dest, *sourceCstr);
        Str_Trunc(dest, -1);
        Str_AddCstr(dest, "o");
        r |= buildSourceToLib(ctx, libDir, lib, dest, source);

        MemCh_Free(m);
        sourceCstr++;
    }
    m->type.range--;

    DebugStack_Pop();
    return r;
}

static status build(BuildCtx *ctx){
    status r = READY;
    DebugStack_Push(NULL, 0);
    MemCh *m = ctx->m;
    setupStatus(ctx);
    Str *libDir = File_GetAbsPath(m, Str_CstrRef(m, ctx->dist));

    Str_AddCstr(libDir, "/");
    Str_AddCstr(libDir, ctx->libtarget);

    Dir_CheckCreate(m, libDir);

    Str *lib = Str_Clone(m, libDir, STR_DEFAULT);
    Str_AddCstr(lib, "/");
    Str_AddCstr(lib, ctx->libtarget);
    Str_AddCstr(lib, ".a");
    if(lib->type.state & ERROR){
        return ERROR;
    }

    BuildSubdir **dir = ctx->objdirs;
    while(*dir != NULL){
        r |= buildDirToLib(ctx, libDir, lib, *dir);
        ctx->fields.steps.modCount++;
        dir++;
    }
    CliStatus_PrintFinish(OutStream, ctx->cli);

    Str *dist = File_GetAbsPath(m, Str_CstrRef(m, ctx->dist));
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
