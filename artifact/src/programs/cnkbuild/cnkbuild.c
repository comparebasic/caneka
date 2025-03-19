#include <external.h>
#include <caneka.h>
#include <cnkbuild.h>

#include "../../inc/core.c"
#include "./debug_mocks.c"


static status renderStatus(MemCtx *m, Abstract *a){
    CliStatus *cli = (CliStatus *)as(a, TYPE_CLI_STATUS);
    BuildCtx *ctx = (BuildCtx *)as(cli->source, TYPE_BUILDCTX);

    String_Reset(ctx->fields.steps.count_s);
    String_Reset(ctx->fields.steps.total_s);
    String_AddInt(m, ctx->fields.steps.count_s, ctx->fields.steps.count);
    String_AddInt(m, ctx->fields.steps.total_s, ctx->fields.steps.total);
    ctx->fields.steps.count_ve->length = ctx->fields.steps.count_s->length;
    ctx->fields.steps.total_ve->length = ctx->fields.steps.total_s->length;

    String_Reset(ctx->fields.steps.modCount_s);
    String_Reset(ctx->fields.steps.modTotal_s);
    String_AddInt(m, ctx->fields.steps.modCount_s, ctx->fields.steps.modCount);
    String_AddInt(m, ctx->fields.steps.modTotal_s, ctx->fields.steps.modTotal);
    ctx->fields.steps.modCount_ve->length = ctx->fields.steps.modCount_s->length;
    ctx->fields.steps.modTotal_ve->length = ctx->fields.steps.modTotal_s->length;

    i32 width = ctx->cli->cols;
    float _progress = ((float)ctx->cli->cols) * ((float)ctx->fields.steps.count)/((float)ctx->fields.steps.total);
    i32 progress = (i32)_progress;
    ctx->fields.steps.barStart->length = progress;

    i32 remainingModSrc =ctx->fields.steps.modSrcTotal - ctx->fields.steps.modSrcCount;
    _progress = ((float)ctx->cli->cols) * ((float)remainingModSrc)/((float)ctx->fields.steps.total);
    progress = (i32)_progress;
    ctx->fields.steps.barEnd->length = progress;

    String_Reset(ctx->fields.mem_s);
    String_AddMemCount(m, ctx->fields.mem_s, MemCount(0));
    ctx->fields.mem->length = ctx->fields.mem_s->length;
     
    return SUCCESS;
}

static status setupStatus(BuildCtx *ctx){
    Span *lines = ctx->cli->lines;
    MemCtx *m = ctx->m;
    memset(&ctx->fields, 0, sizeof(ctx->fields));

    CliStatus_SetDims(ctx->cli, 0, 0);
    i32 width = ctx->cli->cols;

    ctx->fields.steps.modCount_s = String_Init(m, MAX_BASE10+1);
    ctx->fields.steps.modTotal_s = String_Init(m, MAX_BASE10+1);
    StrVec *vh = StrVec_Make(m, bytes("\x1b[33m"), 0);
    StrVec_Add(m, vh, bytes("module "), 0);
    StrVec_Add(m, vh, ctx->fields.steps.modCount_s->bytes, 0);
    ctx->fields.steps.modCount_ve = vh->last;
    StrVec_Add(m, vh, bytes(" of "), 0);
    StrVec_Add(m, vh, ctx->fields.steps.modTotal_s->bytes, 0);
    ctx->fields.steps.modTotal_ve = vh->last;
    StrVec_Add(m, vh, bytes("\x1b[1;33m"), 0);
    StrVec_Add(m, vh, bytes(": "), 0);
    StrVec_Add(m, vh, NULL, 0);
    ctx->fields.steps.name = vh->last;
    Span_Add(ctx->cli->lines, (Abstract *)vh);

    vh = StrVec_Make(m, bytes("\x1b[35m"), 0);
    StrVec_Add(m, vh, NULL, 0);
    ctx->fields.current.action = vh->last;
    StrVec_Add(m, vh, NULL, 0);
    ctx->fields.current.source = vh->last;
    StrVec_Add(m, vh, NULL, 0);
    ctx->fields.current.dest = vh->last;
    StrVec_Add(m, vh, bytes("\x1b[0m"), 0);
    Span_Add(ctx->cli->lines, (Abstract *)vh);

    ctx->fields.steps.count_s = String_Init(m, MAX_BASE10+1);
    ctx->fields.steps.total_s = String_Init(m, MAX_BASE10+1);
    vh = StrVec_Make(m, bytes("sources: "), 0);
    StrVec_Add(m, vh, ctx->fields.steps.count_s->bytes, 0);
    ctx->fields.steps.count_ve = vh->last;
    StrVec_Add(m, vh, bytes(" of "), 0);
    StrVec_Add(m, vh, ctx->fields.steps.total_s->bytes, 0);
    ctx->fields.steps.total_ve = vh->last;
    Span_Add(ctx->cli->lines, (Abstract *)vh);

    Str *s100 = String_Init(m, width);
    memset(s100->bytes, ' ', width);
    vh = StrVec_Make(m, bytes("\x1b[44m"), 0);
    StrVec_Add(m, vh, s100->bytes, 0);
    ctx->fields.steps.barStart = vh->last;
    StrVec_Add(m, vh, bytes("\x1b[43m"), 0);
    StrVec_Add(m, vh, s100->bytes, 0);
    ctx->fields.steps.barEnd = vh->last;
    StrVec_Add(m, vh, bytes("\x1b[0m"), 0);
    Span_Add(ctx->cli->lines, (Abstract *)vh);

    ctx->fields.mem_s = String_Init(m, STRING_EXTEND);
    vh = StrVec_Make(m, bytes("\x1b[0m"), 0);
    StrVec_Add(m, vh, bytes("Memory count: "), 0);
    StrVec_Add(m, vh, ctx->fields.mem_s->bytes, 0);
    ctx->fields.mem = vh->last;
    StrVec_Add(m, vh, bytes("\x1b[0m"), 0);
    Span_Add(ctx->cli->lines, (Abstract *)vh);

    BuildSubdir **dir = ctx->objdirs;
    while(*dir != NULL){
        char **sourceCstr = (*dir)->sources;
        while(*sourceCstr != NULL){
            ctx->fields.steps.total++;
            sourceCstr++;
        }
        dir++;
    }

    return SUCCESS;
}

static status buildExec(BuildCtx *ctx, boolean force, Str *destDir, Str *lib, Executable *target){
    DebugStack_Push(target->bin, TYPE_CSTR);
    status r = READY;
    MemCtx *m = ctx->m;
    Span *cmd = Span_Make(m);
    Span_Add(cmd, (Abstract *)String_Make(m, bytes(ctx->tools.cc)));
    char **ptr = ctx->args.cflags;
    while(*ptr != NULL){
        Span_Add(cmd, (Abstract *)String_Make(m, bytes(*ptr)));
        ptr++;
    }
    ptr = ctx->args.inc;
    while(*ptr != NULL){
        Span_Add(cmd, (Abstract *)String_Make(m, bytes(*ptr)));
        ptr++;
    }
    Span_Add(cmd, (Abstract *)String_Make(m, bytes("-o")));

    Str *dest = String_Init(m, STRING_EXTEND);
    String_Add(m, dest, destDir);
    char *cstr = "/";
    String_AddBytes(m, dest, bytes(cstr), strlen(cstr));
    String_AddBytes(m, dest, bytes(target->bin), strlen(target->bin));
    Span_Add(cmd, (Abstract *)dest);

    Str *source = File_GetAbsPath(m, String_Make(m, bytes(ctx->src)));
    String_AddBytes(m, source, bytes("/programs/"), 1);
    String_AddBytes(m, source, bytes(target->src), strlen(target->src));

    Span_Add(cmd, (Abstract *)source);
    Span_Add(cmd, (Abstract *)lib);
    ptr = ctx->args.libs;
    while(*ptr != NULL){
        Span_Add(cmd, (Abstract *)String_Make(m, bytes(*ptr)));
        ptr++;
    }

    if(force || File_CmpUpdated(m, source, dest, NULL)){
        Debug_Print((void *)dest, 0, "build exec: ", COLOR_CYAN, FALSE);
        printf("\n");

        DebugStack_SetRef(cmd, cmd->type.of);
        ProcDets pd;
        ProcDets_Init(&pd);
        r |= SubProcess(m, cmd, &pd);
        if(r & ERROR){
            Fatal("Build error for source file", 0);
        }

        DebugStack_Pop();
        return r;
    }

    DebugStack_Pop();
    return NOOP;
}

static status buildSourceToLib(BuildCtx *ctx, Str *libDir, Str *lib,Str *dest, Str *source){
    DebugStack_Push(source, source->type.of);
    status r = READY;
    MemCtx *m = ctx->m;
    Span *cmd = Span_Make(m);
    ProcDets pd;
    StrVecEntry_Set(ctx->fields.current.dest, dest->bytes, dest->length);
    ctx->fields.steps.count++;
    ctx->fields.steps.modSrcCount++;
    if(File_CmpUpdated(m, source, dest, NULL)){
        StrVecEntry_Set(ctx->fields.current.action, bytes("build obj: "), 0);
        CliStatus_Print(DebugM, ctx->cli);
        Span_Add(cmd, (Abstract *)String_Make(m, bytes(ctx->tools.cc)));
        char **ptr = ctx->args.cflags;
        while(*ptr != NULL){
            Span_Add(cmd, (Abstract *)String_Make(m, bytes(*ptr)));
            ptr++;
        }
        ptr = ctx->args.inc;
        while(*ptr != NULL){
            Span_Add(cmd, (Abstract *)String_Make(m, bytes(*ptr)));
            ptr++;
        }
        Span_Add(cmd, (Abstract *)String_Make(m, bytes("-c")));
        Span_Add(cmd, (Abstract *)String_Make(m, bytes("-o")));
        Span_Add(cmd, (Abstract *)dest);
        Span_Add(cmd, (Abstract *)source);

        ProcDets_Init(&pd);
        r |= SubProcess(m, cmd, &pd);
        if(r & ERROR){
            DebugStack_SetRef(cmd, cmd->type.of);
            Fatal("Build error for source file", 0);
        }

        Span_ReInit(cmd);
        Span_Add(cmd, (Abstract *)String_Make(m, bytes(ctx->tools.ar)));
        Span_Add(cmd, (Abstract *)String_Make(m, bytes("-rc")));
        Span_Add(cmd, (Abstract *)lib);
        Span_Add(cmd, (Abstract *)dest);
        ProcDets_Init(&pd);
        status re = SubProcess(m, cmd, &pd);
        if(re & ERROR){
            DebugStack_SetRef(cmd, cmd->type.of);
            Fatal("Build error for adding object to lib ", 0);
        }
    }else{
        StrVecEntry_Set(ctx->fields.current.action, bytes(" link obj:"), 0);
        CliStatus_Print(DebugM, ctx->cli);
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
    MemCtx *m = ctx->m;
    Str *dirPath = String_Init(m, STRING_EXTEND);
    String_Add(m, dirPath, libDir);
    char *cstr = "/";
    String_AddBytes(m, dirPath, bytes(cstr), strlen(cstr));
    String_AddBytes(m, dirPath, bytes(dir->name), strlen(dir->name));
    DebugStack_SetRef(dirPath, dirPath->type.of);
    Dir_CheckCreate(m, dirPath);

    Str *source = File_GetAbsPath(m, String_Make(m, bytes(ctx->src)));
    String_AddBytes(m, source, bytes("/"), 1);
    String_AddBytes(m, source, bytes(dir->name), strlen(dir->name));
    String_AddBytes(m, source, bytes("/"), 1);
    i64 sourceL = String_Length(source);

    Str *dest = String_Init(m, STRING_EXTEND);
    String_Add(m, dest, dirPath);
    String_AddBytes(m, dest, bytes("/"), 1);
    i64 destL = String_Length(dest);

    StrVecEntry_Set(ctx->fields.steps.name, bytes(dir->name), 0);

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
        String_Trunc(source, sourceL);
        String_AddBytes(m, source, bytes(*sourceCstr), strlen(*sourceCstr));
        String_Trunc(dest, destL);
        String_AddBytes(m, dest, bytes(*sourceCstr), strlen(*sourceCstr));
        String_Trunc(dest, String_Length(dest)-1);
        String_AddBytes(m, dest, bytes("o"), 1);

        r |= buildSourceToLib(ctx, libDir, lib, dest, source);

        MemCtx_Free(m);
        sourceCstr++;
    }
    m->type.range--;

    DebugStack_Pop();
    return r;
}

static status build(BuildCtx *ctx){
    status r = READY;
    DebugStack_Push(NULL, 0);
    MemCtx *m = ctx->m;
    setupStatus(ctx);
    Str *libDir = Str_Make(m, STR_DEFAULT);
    Str *dist = File_GetAbsPath(m, Str_CstrRef(m, ctx->dist));
    String_Add(m, libDir, dist);
    char *cstr = "/";
    String_AddBytes(m, libDir, bytes(cstr), strlen(cstr));
    String_AddBytes(m, libDir, bytes(ctx->libtarget), strlen(ctx->libtarget));

    Dir_CheckCreate(m, libDir);

    Str *lib = String_Init(m, STRING_EXTEND);
    String_Add(m, lib, libDir);
    cstr = "/";
    String_AddBytes(m, lib, bytes(cstr), strlen(cstr));
    String_AddBytes(m, lib, bytes(ctx->libtarget), strlen(ctx->libtarget));
    cstr = ".a";
    String_AddBytes(m, lib, bytes(cstr), strlen(cstr));

    BuildSubdir **dir = ctx->objdirs;

    ctx->fields.steps.modCount = 0;
    ctx->fields.steps.modTotal = 0;
    while(*dir != NULL){
        ctx->fields.steps.modTotal++;
        dir++;
    }

    dir = ctx->objdirs;
    while(*dir != NULL){
        r |= buildDirToLib(ctx, libDir, lib, *dir);
        ctx->fields.steps.modCount++;
        dir++;
    }
    CliStatus_PrintFinish(DebugM, ctx->cli);

    Executable *target = ctx->targets;
    while(target->bin != NULL){
        buildExec(ctx, ((r & SUCCESS) != 0), dist, lib, target);
        target++;
    }

    DebugStack_Pop();
    return r;
}

status Init(MemCtx *m){
    status r = READY;
    r |= Debug_Init(m);
    r |= DebugStack_Init(m);
    return r;
}

status BuildCtx_Init(MemCtx *m, BuildCtx *ctx){
    Init(m);
    memset(ctx, 0, sizeof(BuildCtx));
    ctx->type.of = TYPE_BUILDCTX;
    ctx->m = MemCtx_Make();
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
