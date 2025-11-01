#include <external.h>
#include <caneka.h>

status BinSeg_Tests(MemCh *gm){
    DebugStack_Push(NULL, 0);
    MemCh *m = MemCh_Make();
    status r = READY;
    Abstract *args[5];
    
    Buff *bf = Buff_Make(m, ZERO);
    BinSegCtx *ctx = BinSegCtx_Make(bf, NULL, NULL, ZERO);

    Str *one = Str_CstrRef(m, "Value Alpha One First Top Rockin!");
    BinSegCtx_Send(ctx, (Abstract *)one, ctx->func(ctx, NULL));

    Str *two = Str_CstrRef(m, "And here is the description.");
    BinSegCtx_Send(ctx, (Abstract *)two, ctx->func(ctx, NULL));

    ctx->keys = Table_Make(m);
    Str *key1 = Str_CstrRef(m, "title");
    Table_Set(ctx->keys, (Abstract *)I16_Wrapped(m, 0),
        (Abstract *)key1);
    Str *key2 = Str_CstrRef(m, "descr");
    Table_Set(ctx->keys, (Abstract *)I16_Wrapped(m, 1),
        (Abstract *)key2);
     
    BinSegCtx_Load(ctx);

    r |= Test((ctx->type.state & (SUCCESS|END)) == (SUCCESS|END), 
        "Ctx finished with status SUCCESS|END", NULL);
    Table *tbl = ctx->tbl;
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
    
    Buff *bf = Buff_Make(m, ZERO);
    BinSegCtx *ctx = BinSegCtx_Make(bf, NULL, NULL, ZERO);

    Str *one = Str_CstrRef(m, "Value Alpha One First Top Rockin!");
    Str *two = Str_CstrRef(m, "Boo Value Two, not as good.");
    Str *three = Str_CstrRef(m, "Maybe Three is a new beginning?");
    Span *p = Span_Make(m);
    i16 spanId = ctx->func(ctx, NULL);
    i16 tableId = ctx->func(ctx, NULL);
    Span_Add(p, (Abstract *)one);
    Span_Add(p, (Abstract *)two);
    Span_Add(p, (Abstract *)three);
    BinSegCtx_Send(ctx, (Abstract *)p, spanId);

    Str *keyA = Str_CstrRef(m, "A");
    Str *apple = Str_CstrRef(m, "Alphabet");
    Str *keyB = Str_CstrRef(m, "B");
    Str *bannana = Str_CstrRef(m, "Baking");
    Str *keyC = Str_CstrRef(m, "C");
    Str *carrot = Str_CstrRef(m, "Casualty");
    Str *keyD = Str_CstrRef(m, "D");
    Str *dice = Str_CstrRef(m, "Delaware");
    Table *tbl = Table_Make(m);
    Table_Set(tbl, (Abstract *)keyA, (Abstract *)apple);
    Table_Set(tbl, (Abstract *)keyB, (Abstract *)bannana);
    Table_Set(tbl, (Abstract *)keyC, (Abstract *)carrot);
    Table_Set(tbl, (Abstract *)keyD, (Abstract *)dice);
    BinSegCtx_Send(ctx, (Abstract *)tbl, tableId);

    ctx->keys = Table_Make(m);
    Str *key1 = Str_CstrRef(m, "list");
    Table_Set(ctx->keys, (Abstract *)I16_Wrapped(m, spanId),
        (Abstract *)key1);
    Str *key2 = Str_CstrRef(m, "dict");
    Table_Set(ctx->keys, (Abstract *)I16_Wrapped(m, tableId),
        (Abstract *)key2);
     
    BinSegCtx_Load(ctx);

    r |= Test((ctx->type.state & (SUCCESS|END)) == (SUCCESS|END), 
        "Ctx finished with status SUCCESS|END", NULL);

    Span *list = (Span *)Table_Get(ctx->tbl, (Abstract *)key1);
    Str *s = (Str *)Span_Get(list, 0);
    r |= Test(Equals((Abstract *)s, (Abstract *)one),
        "first value is expected", NULL);
    s = (Str *)Span_Get(list, 1);
    r |= Test(Equals((Abstract *)s, (Abstract *)two),
        "second value is expected", NULL);
    s = (Str *)Span_Get(list, 2);
    r |= Test(Equals((Abstract *)s, (Abstract *)three),
        "third value is expected", NULL);

    Table *dict = (Span *)Table_Get(ctx->tbl, (Abstract *)key2);
    s = (Str *)Table_Get(dict, (Abstract *)keyC);
    args[0] = (Abstract *)carrot;
    args[1] = (Abstract *)s;
    args[2] = NULL;
    r |= Test(Equals((Abstract *)s, (Abstract *)carrot),
        "keyC value is equal, expected @, have &", args);

    s = (Str *)Table_Get(dict, (Abstract *)keyD);
    args[0] = (Abstract *)dice;
    args[1] = (Abstract *)s;
    args[2] = NULL;
    r |= Test(Equals((Abstract *)s, (Abstract *)dice),
        "keyD value is equal, expected @, have &", args);


    MemCh_Free(m);
    DebugStack_Pop();
    return r;
}

status BinSegV_Tests(MemCh *gm){
    DebugStack_Push(NULL, 0);
    MemCh *m = MemCh_Make();
    status r = READY;
    Abstract *args[5];
    
    Buff *bf = Buff_Make(m, ZERO);
    BinSegCtx *ctx = BinSegCtx_Make(bf, NULL, NULL, BINSEG_VISIBLE);

    Str *one = Str_CstrRef(m, "Value Alpha One First Top Rockin!");
    Str *two = Str_CstrRef(m, "Boo Value Two, not as good.");
    Str *three = Str_CstrRef(m, "Maybe Three is a new beginning?");
    Span *p = Span_Make(m);
    i16 spanId = ctx->func(ctx, NULL);
    i16 tableId = ctx->func(ctx, NULL);
    Span_Add(p, (Abstract *)one);
    Span_Add(p, (Abstract *)two);
    Span_Add(p, (Abstract *)three);
    BinSegCtx_Send(ctx, (Abstract *)p, spanId);

    Str *keyA = Str_CstrRef(m, "A");
    Str *apple = Str_CstrRef(m, "Alphabet");
    Str *keyB = Str_CstrRef(m, "B");
    Str *bannana = Str_CstrRef(m, "Baking");
    Str *keyC = Str_CstrRef(m, "C");
    Str *carrot = Str_CstrRef(m, "Casualty");
    Str *keyD = Str_CstrRef(m, "D");
    Str *dice = Str_CstrRef(m, "Delaware");
    Table *tbl = Table_Make(m);
    Table_Set(tbl, (Abstract *)keyA, (Abstract *)apple);
    Table_Set(tbl, (Abstract *)keyB, (Abstract *)bannana);
    Table_Set(tbl, (Abstract *)keyC, (Abstract *)carrot);
    Table_Set(tbl, (Abstract *)keyD, (Abstract *)dice);
    BinSegCtx_Send(ctx, (Abstract *)tbl, tableId);

    ctx->keys = Table_Make(m);
    Str *key1 = Str_CstrRef(m, "list");
    Table_Set(ctx->keys, (Abstract *)I16_Wrapped(m, spanId),
        (Abstract *)key1);
    Str *key2 = Str_CstrRef(m, "dict");
    Table_Set(ctx->keys, (Abstract *)I16_Wrapped(m, tableId),
        (Abstract *)key2);

    BinSegCtx_Load(ctx);

    r |= Test((ctx->type.state & (SUCCESS|END)) == (SUCCESS|END), 
        "Ctx finished with status SUCCESS|END", NULL);

    Span *list = (Span *)Table_Get(ctx->tbl, (Abstract *)key1);
    Str *s = (Str *)Span_Get(list, 0);
    r |= Test(Equals((Abstract *)s, (Abstract *)one),
        "first value is expected", NULL);
    s = (Str *)Span_Get(list, 1);
    r |= Test(Equals((Abstract *)s, (Abstract *)two),
        "second value is expected", NULL);
    s = (Str *)Span_Get(list, 2);
    r |= Test(Equals((Abstract *)s, (Abstract *)three),
        "third value is expected", NULL);

    Table *dict = (Span *)Table_Get(ctx->tbl, (Abstract *)key2);
    s = (Str *)Table_Get(dict, (Abstract *)keyC);
    args[0] = (Abstract *)carrot;
    args[1] = (Abstract *)s;
    args[2] = NULL;
    r |= Test(Equals((Abstract *)s, (Abstract *)carrot),
        "keyC value is equal, expected @, have &", args);

    s = (Str *)Table_Get(dict, (Abstract *)keyD);
    args[0] = (Abstract *)dice;
    args[1] = (Abstract *)s;
    args[2] = NULL;
    r |= Test(Equals((Abstract *)s, (Abstract *)dice),
        "keyD value is equal, expected @, have &", args);

    MemCh_Free(m);
    DebugStack_Pop();
    return r;
}

status BinSegReversedV_Tests(MemCh *gm){
    DebugStack_Push(NULL, 0);
    MemCh *m = MemCh_Make();
    status r = READY;
    Abstract *args[5];
    
    Buff *bf = Buff_Make(m, ZERO);
    BinSegCtx *ctx = BinSegCtx_Make(bf, NULL, NULL, (BINSEG_REVERSED|BINSEG_VISIBLE));

    Str *one = Str_CstrRef(m, "Value Alpha One First Top Rockin!");
    Str *two = Str_CstrRef(m, "Boo Value Two, not as good.");
    Str *three = Str_CstrRef(m, "Maybe Three is a new beginning?");
    Span *p = Span_Make(m);
    i16 spanId = ctx->func(ctx, NULL);
    i16 tableId = ctx->func(ctx, NULL);
    Span_Add(p, (Abstract *)one);
    Span_Add(p, (Abstract *)two);
    Span_Add(p, (Abstract *)three);
    BinSegCtx_Send(ctx, (Abstract *)p, spanId);

    Str *keyA = Str_CstrRef(m, "A");
    Str *apple = Str_CstrRef(m, "Alphabet");
    Str *keyB = Str_CstrRef(m, "B");
    Str *bannana = Str_CstrRef(m, "Baking");
    Str *keyC = Str_CstrRef(m, "C");
    Str *carrot = Str_CstrRef(m, "Casualty");
    Str *keyD = Str_CstrRef(m, "D");
    Str *dice = Str_CstrRef(m, "Delaware");
    Table *tbl = Table_Make(m);
    Table_Set(tbl, (Abstract *)keyA, (Abstract *)apple);
    Table_Set(tbl, (Abstract *)keyB, (Abstract *)bannana);
    Table_Set(tbl, (Abstract *)keyC, (Abstract *)carrot);
    Table_Set(tbl, (Abstract *)keyD, (Abstract *)dice);
    BinSegCtx_Send(ctx, (Abstract *)tbl, tableId);

    ctx->keys = Table_Make(m);
    Str *key1 = Str_CstrRef(m, "list");
    Table_Set(ctx->keys, (Abstract *)I16_Wrapped(m, spanId),
        (Abstract *)key1);
    Str *key2 = Str_CstrRef(m, "dict");
    Table_Set(ctx->keys, (Abstract *)I16_Wrapped(m, tableId),
        (Abstract *)key2);

    BinSegCtx_Load(ctx);

    r |= Test((ctx->type.state & (SUCCESS|END)) == (SUCCESS|END), 
        "Ctx finished with status SUCCESS|END", NULL);

    Span *list = (Span *)Table_Get(ctx->tbl, (Abstract *)key1);
    Str *s = (Str *)Span_Get(list, 0);
    r |= Test(Equals((Abstract *)s, (Abstract *)one),
        "first value is expected", NULL);
    s = (Str *)Span_Get(list, 1);
    r |= Test(Equals((Abstract *)s, (Abstract *)two),
        "second value is expected", NULL);
    s = (Str *)Span_Get(list, 2);
    r |= Test(Equals((Abstract *)s, (Abstract *)three),
        "third value is expected", NULL);

    Table *dict = (Span *)Table_Get(ctx->tbl, (Abstract *)key2);
    s = (Str *)Table_Get(dict, (Abstract *)keyC);
    args[0] = (Abstract *)carrot;
    args[1] = (Abstract *)s;
    args[2] = NULL;
    r |= Test(Equals((Abstract *)s, (Abstract *)carrot),
        "keyC value is equal, expected @, have &", args);

    s = (Str *)Table_Get(dict, (Abstract *)keyD);
    args[0] = (Abstract *)dice;
    args[1] = (Abstract *)s;
    args[2] = NULL;
    r |= Test(Equals((Abstract *)s, (Abstract *)dice),
        "keyD value is equal, expected @, have &", args);

    MemCh_Free(m);
    DebugStack_Pop();
    return r;
}
