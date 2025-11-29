#include <external.h>
#include <caneka.h>
#include "module.h"

status BuildCli_RenderStatus(MemCh *m, void *a){
    CliStatus *cli = (CliStatus *)as(a, TYPE_CLI_STATUS);
    BuildCtx *ctx = (BuildCtx *)as(cli->source, TYPE_BUILDCTX);

    i32 width = ctx->cli.cli->cols;
    float total = (float)ctx->cli.fields.steps.total->val.i;
    float count = (float)ctx->cli.fields.steps.count->val.i;
    float colsFloat = (float)width;

    float progress = ceil((count/total) * colsFloat);
    ctx->cli.fields.steps.barStart->length = (i32)progress;
    ctx->cli.fields.steps.barEnd->length = width-(i32)progress;

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

status BuildCli_SetupComplete(BuildCtx *ctx){
    FmtLine *ln = Span_Get(ctx->cli.cli->lines, 0);
    ln->fmt = "^g.Completed ^D.$^d. sources^0.";
    ln->args = Arr_Make(ctx->m, 2);
    ln->args[0] = ctx->cli.fields.steps.total;

    ln = Span_Get(ctx->cli.cli->lines, 1);
    ln->fmt = "^g.Static Library ^D.$^d.^0.";

    ln = Span_Get(ctx->cli.cli->lines, 2);
    ctx->cli.fields.steps.barStart->length = ctx->cli.cli->cols;
    ln->fmt = "^G.$^0.";

    ln = Span_Get(ctx->cli.cli->lines, 3);
    ln->args[0] = Str_Ref(ctx->m, (byte *)"g.", 2, 3, STRING_FMT_ANSI);

    return SUCCESS;
}

status BuildCli_SetupStatus(BuildCtx *ctx){
    MemCh *m = ctx->m;
    Span *lines = ctx->cli.cli->lines;
    memset(&ctx->cli.fields, 0, sizeof(ctx->cli.fields));
    ctx->cli.fields.steps.total = I32_Wrapped(m, 0);
    ctx->cli.fields.steps.count = I32_Wrapped(m, 0);
    ctx->cli.fields.steps.name = Str_Make(m, STR_DEFAULT);

    CliStatus_SetDims(ctx->cli.cli, 0, 0);
    i32 width = ctx->cli.cli->cols;
    IntPair coords = {0, 0};

    void **arr = NULL;

    arr = Arr_Make(m, 2);
    arr[0] = ctx->cli.fields.steps.count;
    arr[1] = ctx->cli.fields.steps.total;
    Span_Add(ctx->cli.cli->lines, 
        FmtLine_Make(m, "^y.Source $ of $^0", arr));

    ctx->cli.fields.current[4] = NULL;
    Span_Add(ctx->cli.cli->lines, 
        FmtLine_Make(m, "^y.Building $ $ $", ctx->cli.fields.current));

    ctx->cli.fields.steps.barStart = Str_Make(m, width);
    memset(ctx->cli.fields.steps.barStart->bytes, ' ', width);
    ctx->cli.fields.steps.barStart->length = width;
    ctx->cli.fields.steps.barEnd = Str_CloneAlloc(m, ctx->cli.fields.steps.barStart, width);
    ctx->cli.fields.steps.barStart->length = 0;
    
    arr = Arr_Make(m, 2);
    arr[0] = ctx->cli.fields.steps.barStart; 
    arr[1] = ctx->cli.fields.steps.barEnd;
    Span_Add(ctx->cli.cli->lines,
        FmtLine_Make(m, "^B.$^0.^Y.$^0", arr));

    arr = Arr_Make(m, 5);
    arr[0] = NULL;
    arr[1] = I64_Wrapped(m, 0);
    arr[2] = I64_Wrapped(m, 0);
    arr[3] = I64_Wrapped(m, PAGE_SIZE);
    arr[4] = NULL;
    Span_Add(ctx->cli.cli->lines,
        FmtLine_Make(m,
            "^c.Memory $ total/maxIdx=^D.$/$^d. page-size=$b^0", arr));

    coords.a = ctx->cli.cli->lines->max_idx;
    coords.b = 0;
    CliStatus_SetKey(m, ctx->cli.cli, Str_CstrRef(m, "memTotal"), &coords);
    coords.b = 1;
    CliStatus_SetKey(m, ctx->cli.cli, Str_CstrRef(m, "chapters"), &coords);
    coords.b = 2;
    CliStatus_SetKey(m, ctx->cli.cli, Str_CstrRef(m, "chaptersTotal"), &coords);

    return SUCCESS;
}
