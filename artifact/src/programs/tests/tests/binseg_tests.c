#include <external.h>
#include <caneka.h>

status BinSeg_Tests(MemCh *gm){
    DebugStack_Push(NULL, 0);
    MemCh *m = MemCh_Make();
    status r = READY;
    Abstract *args[5];
    
    Stream *sm = Stream_MakeStrVec(m);
    BinSegCtx *ctx = BinSegCtx_Make(sm, NULL, NULL);

    Str *one = Str_CstrRef(m, "Value Alpha One First Top Rockin!");
    BinSegCtx_ToStream(ctx, (Abstract *)one, ctx->func(ctx, NULL));

    Str *two = Str_CstrRef(m, "And here is the description.");
    BinSegCtx_ToStream(ctx, (Abstract *)two, ctx->func(ctx, NULL));

    ctx->keys = Table_Make(m);
    Str *key1 = Str_CstrRef(m, "title");
    Table_Set(ctx->keys, (Abstract *)I16_Wrapped(m, 0),
        (Abstract *)key1);
    Str *key2 = Str_CstrRef(m, "descr");
    Table_Set(ctx->keys, (Abstract *)I16_Wrapped(m, 1),
        (Abstract *)key2);
     
    BinSegCtx_LoadStream(ctx);

    r |= Test((ctx->type.state & (SUCCESS|END)) == (SUCCESS|END), 
        "Ctx finished with status SUCCESS|END", NULL);
    Table *tbl = ctx->tblIt.p;
    r |= Test(Equals((Abstract *)Table_Get(tbl, (Abstract *)key1), (Abstract *)one), 
        "first value is expected", NULL);
    r |= Test(Equals((Abstract *)Table_Get(tbl, (Abstract *)key2), (Abstract *)two), 
        "second value is expected", NULL);

    MemCh_Free(m);
    DebugStack_Pop();
    return r;
}

status BinSegCollection_Tests(MemCh *gm){
    DebugStack_Push(NULL, 0);
    MemCh *m = MemCh_Make();
    status r = READY;
    Abstract *args[5];
    
    Stream *sm = Stream_MakeStrVec(m);
    BinSegCtx *ctx = BinSegCtx_Make(sm, NULL, NULL);

    Str *one = Str_CstrRef(m, "Value Alpha One First Top Rockin!");
    Str *two = Str_CstrRef(m, "And here is the description.");
    Str *three = Str_CstrRef(m, "And here is the description.");
    Span *p = Span_Make(m);
    Span_Add(p, (Abstract *)one);
    Span_Add(p, (Abstract *)two);
    Span_Add(p, (Abstract *)three);

    BinSegCtx_ToStream(ctx, (Abstract *)p, ctx->func(ctx, NULL));

    args[0] = (Abstract *)ctx;
    args[1] = NULL;
    Out("^y.Ctx after collection &\n", args);

    /*
    ctx->keys = Table_Make(m);
    Str *key1 = Str_CstrRef(m, "list");
    Table_Set(ctx->keys, (Abstract *)I16_Wrapped(m, 0),
        (Abstract *)key1);
     
    BinSegCtx_LoadStream(ctx);
    r |= Test((ctx->type.state & (SUCCESS|END)) == (SUCCESS|END), 
        "Ctx finished with status SUCCESS|END", NULL);
    Span *list = (Span *)Table_Get(ctx->tblIt.p, (Abstract *)key1);
    Str *s = (Str *)Span_Get(list, 0);
    r |= Test(Equals((Abstract *)s, (Abstract *)one),
        "first value is expected", NULL);
    s = (Str *)Span_Get(list, 1);
    r |= Test(Equals((Abstract *)s, (Abstract *)two),
        "second value is expected", NULL);
    s = (Str *)Span_Get(list, 2);
    r |= Test(Equals((Abstract *)s, (Abstract *)three),
        "third value is expected", NULL);
    */

    MemCh_Free(m);
    DebugStack_Pop();
    return r;
}
