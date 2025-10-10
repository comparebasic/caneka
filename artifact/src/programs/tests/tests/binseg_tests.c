#include <external.h>
#include <caneka.h>

status BinSeg_Tests(MemCh *gm){
    DebugStack_Push(NULL, 0);
    MemCh *m = MemCh_Make();
    status r = READY;
    Abstract *args[5];
    
    Str *s = Str_CstrRef(m, "Value Alpha One First Top Rockin!");
    Stream *sm = Stream_MakeStrVec(m);
    BinSegCtx *ctx = BinSegCtx_Make(sm, NULL, NULL);
    BinSegCtx_ToStream(ctx, (Abstract *)s);

    args[0] = (Abstract *)ctx;
    args[1] = NULL;
    Out("^p.Content from BinSeg &^0\n", args);

    ctx->type.state |= DEBUG;
    ctx->keys = Table_Make(m);
    Table_Set(ctx->keys, (Abstract *)I16_Wrapped(m, 0),
        (Abstract *)Str_CstrRef(m, "title"));
    Table_Set(ctx->keys, (Abstract *)I16_Wrapped(m, 1),
        (Abstract *)Str_CstrRef(m, "descr"));
     
    BinSegCtx_LoadStream(ctx);

    args[0] = (Abstract *)ctx;
    args[1] = NULL;
    Out("^p.Content from BinSeg &^0\n", args);

    MemCh_Free(m);
    DebugStack_Pop();
    return r;
}
