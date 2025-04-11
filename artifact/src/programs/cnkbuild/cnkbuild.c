#include <external.h>
#include <caneka.h>
#include <cnkbuild.h>


static status renderStatus(MemCh *m, Abstract *a){
    CliStatus *cli = (CliStatus *)as(a, TYPE_CLI_STATUS);
    BuildCtx *ctx = (BuildCtx *)as(cli->source, TYPE_BUILDCTX);

    Str_Reset(ctx->fields.steps.count_s);
    Str_Reset(ctx->fields.steps.total_s);
    Str_AddI64(ctx->fields.steps.count_s, ctx->fields.steps.count);
    Str_AddI64(ctx->fields.steps.total_s, ctx->fields.steps.total);

    Str_Reset(ctx->fields.steps.modCount_s);
    Str_Reset(ctx->fields.steps.modTotal_s);
    Str_AddI64(ctx->fields.steps.modCount_s, ctx->fields.steps.modCount);
    Str_AddI64(ctx->fields.steps.modTotal_s, ctx->fields.steps.modTotal);

    i32 width = ctx->cli->cols;
    float _progress = ((float)ctx->cli->cols) * ((float)ctx->fields.steps.count)/((float)ctx->fields.steps.total);
    i32 progress = (i32)_progress;
    ctx->fields.steps.barStart->length = progress;

    i32 remainingModSrc = ctx->fields.steps.modSrcTotal - ctx->fields.steps.modSrcCount;
    _progress = ((float)ctx->cli->cols) * ((float)remainingModSrc)/((float)ctx->fields.steps.total);
    progress = (i32)_progress;
    ctx->fields.steps.barEnd->length = progress;

    Str_Reset(ctx->fields.mem_s);
    Str_AddMemCount(ctx->fields.mem_s, MemCount(0));
     
    return SUCCESS;
}

static status setupStatus(BuildCtx *ctx){
    Span *lines = ctx->cli->lines;
    MemCh *m = ctx->m;
    memset(&ctx->fields, 0, sizeof(ctx->fields));

    CliStatus_SetDims(ctx->cli, 0, 0);
    i32 width = ctx->cli->cols;
    IntPair coords = {0, 0};

    Stream *sm = Stream_MakeStrVec(m);
    StrVec *v = sm->dest.curs->v;
    ctx->fields.steps.modCount_s = Str_Make(m, MAX_BASE10+1);
    ctx->fields.steps.modTotal_s = Str_Make(m, MAX_BASE10+1);
    ctx->fields.steps.name = Str_Make(m, STR_DEFAULT);
    StrVec_Fmt(sm, "^ymodule _t of _t: ^yD._t^0", ctx->fields.steps.modCount_s, ctx->fields.steps.modTotal_s, ctx->fields.steps.name);
    Span_Add(ctx->cli->lines, (Abstract *)v);

    coords.a = ctx->cli->lines->max_idx;
    coords.b = StrVec_GetIdx(v, ctx->fields.steps.modCount_s);
    CliStatus_SetKey(m, ctx->cli, Str_CstrRef(m, "modCount"), &coords);
    coords.b = StrVec_GetIdx(v, ctx->fields.steps.modTotal_s);
    CliStatus_SetKey(m, ctx->cli, Str_CstrRef(m, "modTotal"), &coords);

    sm = Stream_MakeStrVec(m);
    v = sm->dest.curs->v;
    ctx->fields.current.action = Str_Make(m, STR_DEFAULT);
    ctx->fields.current.source = Str_Make(m, STR_DEFAULT);
    ctx->fields.current.dest = Str_Make(m, STR_DEFAULT);
    StrVec_Fmt(sm, "^b_t: _t -> ^bD._t^0", ctx->fields.current.action, ctx->fields.current.source, ctx->fields.current.dest);
    Span_Add(ctx->cli->lines, (Abstract *)v);

    coords.a = ctx->cli->lines->max_idx;
    coords.b = StrVec_GetIdx(v, ctx->fields.current.action);
    CliStatus_SetKey(m, ctx->cli, Str_CstrRef(m, "action"), &coords);
    coords.b = StrVec_GetIdx(v, ctx->fields.current.source);
    CliStatus_SetKey(m, ctx->cli, Str_CstrRef(m, "source"), &coords);
    coords.b = StrVec_GetIdx(v, ctx->fields.current.dest);
    CliStatus_SetKey(m, ctx->cli, Str_CstrRef(m, "dest"), &coords);

    sm = Stream_MakeStrVec(m);
    v = sm->dest.curs->v;
    ctx->fields.steps.count_s = Str_Make(m, MAX_BASE10+1);
    ctx->fields.steps.total_s = Str_Make(m, MAX_BASE10+1);
    StrVec_Fmt(sm, "sources: _t of _t^0", ctx->fields.steps.count_s, ctx->fields.steps.total_s);
    Span_Add(ctx->cli->lines, (Abstract *)v);

    coords.a = ctx->cli->lines->max_idx;
    coords.b = StrVec_GetIdx(v, ctx->fields.steps.count_s);
    CliStatus_SetKey(m, ctx->cli, Str_CstrRef(m, "stepAction"), &coords);
    coords.b = StrVec_GetIdx(v, ctx->fields.steps.total_s);
    CliStatus_SetKey(m, ctx->cli, Str_CstrRef(m, "stepTotal"), &coords);

    ctx->fields.steps.barStart = Str_Make(m, width);
    memset(ctx->fields.steps.barStart->bytes, ' ', width);
    ctx->fields.steps.barEnd = Str_Clone(m, ctx->fields.steps.barStart, width);
    sm = Stream_MakeStrVec(m);
    v = sm->dest.curs->v;
    StrVec_Fmt(sm, "^B_t^Y_t^0", ctx->fields.steps.barStart,ctx->fields.steps.barEnd);
    Span_Add(ctx->cli->lines, (Abstract *)v);

    sm = Stream_MakeStrVec(m);
    v = sm->dest.curs->v;
    ctx->fields.mem_s = Str_Make(m, STR_DEFAULT);
    StrVec_Fmt(sm, "^cMemory Count: _t^0", ctx->fields.mem_s);
    Span_Add(ctx->cli->lines, (Abstract *)v);

    coords.a = ctx->cli->lines->max_idx;
    coords.b = StrVec_GetIdx(v, ctx->fields.mem_s);
    CliStatus_SetKey(m, ctx->cli, Str_CstrRef(m, "memCount"), &coords);

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
    /*
    MemCh *m = ctx->m;
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
    */
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

    Str *source = File_GetAbsPath(m, Str_CstrRef(m, ctx->src));
    Str_AddCstr(source, "/");
    Str_AddCstr(source, dir->name);
    Str_AddCstr(source, "/");
    i64 sourceL = source->length;

    Str *dest = Str_Make(m, STR_DEFAULT);
    Str_Add(dest, dirPath->bytes, dirPath->length);
    Str_AddCstr(dest, "/");
    i64 destL = dest->length;
    Out("^y.Dest: _d^0\n", dest);
    
    /*
    CliStatus_SetByKey(m, ctx->cli, Str_CstrRef(m, "source"), 
        Str_CstrRef(m, dir->name));  
        */

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
        Out("^p.Building: _d -> _d^0\n", source, dest);

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
    CliStatus_PrintFinish(_debugM, ctx->cli);

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
