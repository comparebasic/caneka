#include <external.h>
#include <caneka.h>
#include <builder.h>

static status renderStatus(MemCh *m, void *a){
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

    MemBookStats st;
    MemBook_GetStats(m, &st);

    CliStatus_SetByKey(m, cli, Str_CstrRef(m, "memTotal"),
        Str_MemCount(ctx->m, st.total*PAGE_SIZE));

    Single *sg = (Single *)as(CliStatus_GetByKey(m, 
        cli, Str_CstrRef(m, "chapters")), TYPE_WRAPPED_I64);
    sg->val.value = st.total;

    sg = (Single *)as(CliStatus_GetByKey(m, 
        cli, Str_CstrRef(m, "chaptersTotal")), TYPE_WRAPPED_I64);
    sg->val.value = st.pageIdx;

    return SUCCESS;
}

static status setupComplete(BuildCtx *ctx){
    FmtLine *ln = Span_Get(ctx->cli->lines, 0);
    ln->fmt = "^g.Complete - ^D.$^d.sources/^D.$^d.modules^0.";
    ln->args = Arr_Make(ctx->m, 2);
    ln->args[0] = ctx->fields.steps.total;
    ln->args[1] = ctx->fields.steps.modTotal;

    ln = Span_Get(ctx->cli->lines, 1);
    ln->fmt = "";
    ln = Span_Get(ctx->cli->lines, 2);
    ln->fmt = "";

    ln = Span_Get(ctx->cli->lines, 3);
    ctx->fields.steps.barStart->length = ctx->cli->cols;
    ln->fmt = "^G.$^0.";

    ln = Span_Get(ctx->cli->lines, 4);
    ln->args[0] = Str_Ref(ctx->m, (byte *)"g.", 2, 3, STRING_FMT_ANSI);

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
    while(dir != NULL && *dir != NULL){
        char **sourceCstr = (*dir)->sources;
        while(*sourceCstr != NULL){
            ctx->fields.steps.total->val.i++;
            sourceCstr++;
        }
        dir++;
    }

    dir = ctx->objdirs;
    while(dir != NULL && *dir != NULL){
        ctx->fields.steps.modTotal->val.i++;
        dir++;
    }

    if(ctx->args.licenceFiles != NULL){
        ctx->fields.steps.modTotal->val.i++;
    }

    CliStatus_SetDims(ctx->cli, 0, 0);
    i32 width = ctx->cli->cols;
    IntPair coords = {0, 0};

    void **arr = Arr_Make(m, 3);
    arr[0] = ctx->fields.steps.modCount;
    arr[1] = ctx->fields.steps.modTotal;
    arr[2] = ctx->fields.steps.name;
    Span_Add(ctx->cli->lines, 
        FmtLine_Make(m, "^y.module $ of $: ^D.$^0", arr));

    arr = Arr_Make(m, 3);
    arr[0] = ctx->fields.current.action;
    arr[1] = ctx->fields.current.source;

    Span_Add(ctx->cli->lines, 
        FmtLine_Make(m, "^b.$: $", arr));

    arr = Arr_Make(m, 1);
    arr[0] = ctx->fields.current.dest;
    Span_Add(ctx->cli->lines, 
        FmtLine_Make(m, "    -> ^D.$^0", arr));

    ctx->fields.steps.barStart = Str_Make(m, width);
    memset(ctx->fields.steps.barStart->bytes, ' ', width);
    ctx->fields.steps.barStart->length = width;
    ctx->fields.steps.barEnd = Str_CloneAlloc(m, ctx->fields.steps.barStart, width);
    ctx->fields.steps.barStart->length = 0;
    
    arr = Arr_Make(m, 2);
    arr[0] = ctx->fields.steps.barStart; 
    arr[1] = ctx->fields.steps.barEnd;
    Span_Add(ctx->cli->lines,
        FmtLine_Make(m, "^B.$^0.^Y.$^0", arr));

    arr = Arr_Make(m, 5);
    arr[0] = NULL;
    arr[1] = I64_Wrapped(m, 0);
    arr[2] = I64_Wrapped(m, 0);
    arr[3] = I64_Wrapped(m, PAGE_SIZE);
    arr[4] = NULL;
    Span_Add(ctx->cli->lines,
        FmtLine_Make(m,
            "^c.Memory $ total/maxIdx=^D.$/$^d. page-size=$b^0", arr));

    coords.a = ctx->cli->lines->max_idx;
    coords.b = 0;
    CliStatus_SetKey(m, ctx->cli, Str_CstrRef(m, "memTotal"), &coords);
    coords.b = 1;
    CliStatus_SetKey(m, ctx->cli, Str_CstrRef(m, "chapters"), &coords);
    coords.b = 2;
    CliStatus_SetKey(m, ctx->cli, Str_CstrRef(m, "chaptersTotal"), &coords);

    return SUCCESS;
}

static status buildExec(BuildCtx *ctx, boolean force, Str *destDir, Str *lib, Executable *target){
    DebugStack_Push(target->bin, TYPE_CSTR);
    status r = READY;
    MemCh *m = ctx->m;
    void *args[12];

    args[0] = ctx->source;
    args[1] = "/";
    args[2] = target->src;
    Str *souce = StrVec_Str(m, IoUtil_AbsPathBuilder(m, args));

    args[0] = ctx->dist;
    args[1] = "/bin/";
    args[2] = dest;
    args[2] = target->bin;
    Str *dest = StrVec_Str(m, IoUtil_AbsPathBuilder(m, args));

    ProcDets pd;
    if(1 || IoUtil_CmpUpdated(ctx->m, source, dest)){
        void *args[] = {
            source,
            dest,
            NULL
        };
        Out("^c.Building Executable $ -> $^0.\n", args);

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

        ptr = ctx->args.libs;
        while(*ptr != NULL){
            Span_Add(cmd, Str_CstrRef(ctx->m, *ptr));
            ptr++;
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
    return NOOP;
}

static status buildObject(BuildCtx *ctx,
        Str *libFileName,
        Str *dest,
        Str *source
    ){
    DebugStack_Push(source, source->type.of);
    status r = READY;
    void args[12];
    MemCh *m = ctx->m;
    Span *cmd = Span_Make(m);
    ProcDets pd;

    if(IoUtil_CmpUpdated(m, source, dest)){
        m->level--;
        Str_Reset(ctx->fields.current.action);
        Str_AddCstr(ctx->fields.current.action, "build obj");
        CliStatus_Print(OutStream, ctx->cli);
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
            Str *genDest = IoUtil_AbsPathBuilder(m, args);

            s = Str_Make(m, genDest->total+2);
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
        Str_Reset(ctx->fields.current.action);
        Str_AddCstr(ctx->fields.current.action, " link obj");
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

    CliStatus_Print(OutStream, ctx->cli);

    if(r == READY){
        r = NOOP;
    }

    DebugStack_Pop();
    return r;
}

static status build(BuildCtx *ctx){
    status r = READY;
    DebugStack_Push(NULL, 0);
    void *args[12];
    MemCh *m = ctx->m;

    setupStatus(ctx);

    args[0] = ctx->src;
    args[1] = "/"
    args[2] = NULL;
    StrVec *src = IoUtil_AbsPathBuilder(m, args);;

    lib = S(m, ctx->libtarget);

    StrVec *libDir = IoAbsPath(m, ctx->dist);
    Str_AddCstr(libDir, "/");
    Str_AddCstr(libDir, ctx->libtarget);
    StrVec_Add(m, S(m, "/"));

    Str *libPath = StrVec_Str(m, libDir);

    Dir_CheckCreate(m, libPath);

    Str *targetName = Str_Make(m, strlen(ctx->libtarget)+3);
    Str_AddCstr(m, ctx->libtarget);
    Str_AddCstr(m, ".a");
    StrVec_Add(libDir, targetName);
    Str *libFileName = StrVec_Str(m, libDir);
    StrVec_Pop(libDir);

    File_Unlink(m, libFileName);

    if(ctx->genConfigs != NULL){
        GenConfig *config = ctx->genConfigs;

        args[0] = ctx->src;
        args[1] = "/gen/"
        args[2] = NULL;
        StrVec *genSrc = IoUtil_AbsPathBuilder(m, args);

        args[0] = ctx->dist;
        args[1] = "/"
        args[2] = ctx->libtarget; 
        args[3] = "/include/gen/"; 
        args[4] = NULL;
        StrVec *genDest = IoUtil_AbsPathBuilder(m, args);

        Dir_CheckCreate(m, genDest);

        while(config->file != NULL){

            StrVec_Add(genSrc, S(m, config->file));
            StrVec_Add(genDest, S(m, config->file));
            Str *key = S(m, config->key);

            r |= Generate(m,
                StrVec_Str(m, genSrc), key, config->args, StrVec_Str(m, genDest));
            if(r & ERROR){
                args[0] = genSrc;
                args[1] = genDest;
                args[2] = NULL;
                Fatal(FUNCNAME, FILENAME, LINENUMBER, 
                    "Error generating static file: $ -> $", args);
                return ERROR;
            }

            StrVec_Pop(genSrc);
            StrVec_Pop(genDest);
            config++;
        }
    }

    ctx->fields.steps.modSrcCount->val.i = 0;
    ctx->fields.steps.modSrcTotal->val.i = 0;
    char **sourcePtr = ctx->sources;
    while(sourcePtr != NULL && *sourcePtr != NULL){
        ctx->fields.steps.modSrcTotal->val.i++;
        sourcePtr++;
    }

    i32 libCount = 0;
    char **sourcePtr = ctx->sources;
    while(sourcePtr != NULL && *sourcePtr != NULL){
        m->level++;
        Str *source = S(m, *sourcePtr);
        if(source->bytes[s->length-1] == '/'){
            StrVec_Add(src, source);
            StrVec_Add(libDir, source);
            Str *dir = StrVec_Str(m, libDir);

            Span *p = Span_Make(m);
            Dir_Gather(m, StrVec_Str(m, src), p);

            Iter it;
            Iter_Init(&it, p);
            while((Iter_Next(&it, p) & END) == 0){
                Str *s = Iter_Get(&it);
                StrVec_Add(libDir, s);
                StrVec_Add(src, s);

                Str *sourceSrc = StrVec_Str(m, dest);
                Str *dest = StrVec_Str(m, libDir);
                dest->bytes[dest->length-1] = 'o';
                r |= buildObject(ctx, libFileName, dest, src);

                StrVec_Pop(libDir);
                StrVec_Pop(src);
            }

            StrVec_Pop(libDir);
            StrVec_Pop(src);
        }else{
            Str *s = source;
            StrVec_Add(libDir, s);
            StrVec_Add(src, s);

            Str *sourceSrc = StrVec_Str(m, dest);
            Str *dest = StrVec_Str(m, libDir);
            dest->bytes[dest->length-1] = 'o';
            r |= buildObject(ctx, libFileName, dest, src);

            StrVec_Pop(libDir);
            StrVec_Pop(src);
        }

        sourcePtr++;
        libCount++;
        ctx->fields.steps.count->val.i++;
        ctx->fields.steps.modSrcCount->val.i++;
        m->level--;
        MemCh_FreeTemp(m);
    }

    ctx->fields.steps.modSrcCount->val.i = 0;
    ctx->fields.steps.modSrcTotal->val.i = 0;
    CliStatus_Print(OutStream, ctx->cli);

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
        if(libCount == 0){
            lib = NULL;
        }
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
    ctx->cli = CliStatus_Make(m, renderStatus, ctx);
    return SUCCESS;
}

status Build(BuildCtx *ctx){
    DebugStack_Push(ctx, ctx->type.of);
    status r = READY;
    r |= build(ctx);
    DebugStack_Pop();
    return r;
}
