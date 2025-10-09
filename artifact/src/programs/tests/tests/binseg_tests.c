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
    BinSeg_ToStream(ctx, (Abstract *)s);

    args[0] = (Abstract *)ctx;
    args[1] = NULL;
    Out("^p.Content from BinSeg &^0\n", args);

    BinSeg_LoadStream(ctx);

    MemCh_Free(m);
    DebugStack_Pop();
    return r;
}
