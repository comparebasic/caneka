#include <external.h>
#include <caneka.h>
#include <builder.h>

#include "mem/mem.c"
#include "core/core.c"
#include "core/error.c"
#include "string/string.c"
#include "string/string_makers.c"
#include "debug/debug_flags.c"
#include "debug/debug_typestrings.c"
#include "debug/debug_stack.c"
#include "builder/debug_mocks.c"
#include "crypto/sane.c"
/* span */
#include "sequence/slab.c"
#include "sequence/spandef.c"
#include "sequence/span.c"
#include "sequence/spanquery.c"
#include "sequence/iter.c"
/* file status */
#include "persist/file_status.c"
/* dir */
#include "persist/dir.c"
/* spawn */
#include "persist/procdets.c"
#include "persist/subprocess.c"

static status buildSourceToLib(BuildCtx *ctx, String *libDir, String *lib,String *dest, String *source){
    DebugStack_Push(source, source->type.of);
    status r = READY;
    MemCtx *m = ctx->m;
    Span *cmd = Span_Make(m, TYPE_SPAN);
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

    Debug_Print((void *)dest, 0, "building: ", COLOR_YELLOW, FALSE);
    printf("\n");

    ProcDets pd;
    ProcDets_Init(&pd);
    r |= SubProcess(m, cmd, &pd);
    if(r & ERROR){
        Fatal("Build error for source file", 0);
    }

    Span_ReInit(cmd);
    Span_Add(cmd, (Abstract *)String_Make(m, bytes(ctx->tools.ar)));
    Span_Add(cmd, (Abstract *)String_Make(m, bytes("-rc")));
    Span_Add(cmd, (Abstract *)lib);
    Span_Add(cmd, (Abstract *)dest);
    ProcDets_Init(&pd);
    r |= SubProcess(m, cmd, &pd);
    if(r & ERROR){
        Fatal("Build error for adding object to lib ", 0);
    }

    DebugStack_Pop();
    return r;
}

static status buildDirToLib(BuildCtx *ctx, String *libDir, String *lib, BuildSubdir *dir){
    DebugStack_Push(NULL, 0);
    status r = READY;
    MemCtx *m = ctx->m;
    String *dirPath = String_Init(m, STRING_EXTEND);
    String_Add(m, dirPath, libDir);
    char *cstr = "/";
    String_AddBytes(m, dirPath, bytes(cstr), strlen(cstr));
    String_AddBytes(m, dirPath, bytes(dir->name), strlen(dir->name));
    DebugStack_SetRef(dirPath, dirPath->type.of);
    Dir_CheckCreate(m, dirPath);

    String *source = File_GetAbsPath(m, String_Make(m, bytes(ctx->src)));
    String_AddBytes(m, source, bytes("/"), 1);
    String_AddBytes(m, source, bytes(dir->name), strlen(dir->name));
    String_AddBytes(m, source, bytes("/"), 1);
    i64 sourceL = String_Length(source);

    String *dest = String_Init(m, STRING_EXTEND);
    String_Add(m, dest, dirPath);
    String_AddBytes(m, dest, bytes("/"), 1);
    i64 destL = String_Length(dest);
    char **sourceCstr = dir->sources;

    m->type.range++;
    DebugM->type.range++;
    while(*sourceCstr != NULL){
        String_Trunc(source, sourceL);
        String_AddBytes(m, source, bytes(*sourceCstr), strlen(*sourceCstr));
        String_Trunc(dest, destL);
        String_AddBytes(m, dest, bytes(*sourceCstr), strlen(*sourceCstr));
        String_Trunc(dest, String_Length(dest)-1);
        String_AddBytes(m, dest, bytes("o"), 1);

        buildSourceToLib(ctx, libDir, lib, dest, source);
        MemCtx_Free(m);
        MemCtx_Free(DebugM);
        sourceCstr++;
    }
    DebugM->type.range--;
    m->type.range--;

    DebugStack_Pop();
    return SUCCESS;
}

static status buildLib(BuildCtx *ctx){
    DebugStack_Push(NULL, 0);
    MemCtx *m = ctx->m;
    String *libDir = String_Init(m, STRING_EXTEND);
    String *dist = File_GetAbsPath(m, String_Make(m, bytes(ctx->dist)));
    String_Add(m, libDir, dist);
    char *cstr = "/";
    String_AddBytes(m, libDir, bytes(cstr), strlen(cstr));
    String_AddBytes(m, libDir, bytes(ctx->libtarget), strlen(ctx->libtarget));

    Dir_CheckCreate(m, libDir);

    String *lib = String_Init(m, STRING_EXTEND);
    String_Add(m, lib, libDir);
    cstr = "/";
    String_AddBytes(m, lib, bytes(cstr), strlen(cstr));
    String_AddBytes(m, lib, bytes(ctx->libtarget), strlen(ctx->libtarget));
    cstr = ".a";
    String_AddBytes(m, lib, bytes(cstr), strlen(cstr));

    BuildSubdir **dir = ctx->objdirs;
    while(*dir != NULL){
        buildDirToLib(ctx, libDir, lib, *dir);
        dir++;
    }

    DebugStack_Pop();
    return SUCCESS;
}

status BuildCtx_Init(MemCtx *m, BuildCtx *ctx){
    memset(ctx, 0, sizeof(BuildCtx));
    ctx->type.of = TYPE_BUILDCTX;
    ctx->m = m;
    return SUCCESS;
}

status Build(BuildCtx *ctx){
    status r = READY;
    r |= Debug_Init(ctx->m);
    r |= SpanDef_Init();
    r |= DebugStack_Init(ctx->m);
    DebugStack_Push(ctx, ctx->type.of);
    r |= buildLib(ctx);
    DebugStack_Pop();
    return r;
}
