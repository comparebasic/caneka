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

    i16 vecId = ctx->func(ctx, NULL);
    StrVec *v = StrVec_From(m, Str_FromCstr(m, "one", ZERO));
    StrVec_Add(v, Str_FromCstr(m, "+", ZERO)); 
    StrVec_Add(v, Str_FromCstr(m, "two", ZERO));
    StrVec_Add(v, Str_FromCstr(m, "+", ZERO));
    StrVec_Add(v, Str_FromCstr(m, "three", ZERO));
    StrVec_Add(v, Str_FromCstr(m, "+", ZERO));
    StrVec_Add(v, Str_FromCstr(m, "four", ZERO));
    BinSegCtx_Send(ctx, (Abstract *)v, vecId);

    Str *path = IoUtil_GetAbsPath(m, Str_CstrRef(m, "./examples/object.config"));
    StrVec *content = File_ToVec(m, path);
    Cursor *curs = Cursor_Make(m, content); 
    Roebling *rbl = NULL;
    rbl = FormatFmt_Make(m, curs, NULL);
    rbl->dest->type.state |= DEBUG;
    Roebling_Run(rbl);

    args[0] = (Abstract *)rbl;
    args[1] = NULL;
    r |= Test((rbl->type.state & ERROR) == 0,
        "Object Fmt parsed with status without ERROR @", args);

    args[0] = (Abstract *)rbl->dest;
    args[1] = NULL;
    Out("^y.Mess @^0\n", args);

    ctx->keys = Table_Make(m);
    Str *key1 = Str_CstrRef(m, "list");
    Table_Set(ctx->keys, (Abstract *)I16_Wrapped(m, spanId),
        (Abstract *)key1);
    Str *key2 = Str_CstrRef(m, "dict");
    Table_Set(ctx->keys, (Abstract *)I16_Wrapped(m, tableId),
        (Abstract *)key2);
    Str *key3 = Str_CstrRef(m, "vec");
    Table_Set(ctx->keys, (Abstract *)I16_Wrapped(m, vecId),
        (Abstract *)key3);
     
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

    args[0] = (Abstract *)StrVec_From(m, Str_FromCstr(m, "one+two+three+four", ZERO));
    args[1] = (Abstract *)Table_Get(ctx->tbl, (Abstract *)key3);
    args[2] = NULL;
    r |= Test(Equals((Abstract *)args[1], (Abstract *)args[0]),
        "key3 StrVec value is equal, expected @, have @", args);

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
