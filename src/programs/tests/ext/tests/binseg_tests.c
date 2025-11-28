#include <external.h>
#include <caneka.h>

status BinSeg_Tests(MemCh *m){
    DebugStack_Push(NULL, 0);
    status r = READY;
    void *args[5];
    
    Buff *bf = Buff_Make(m, ZERO);
    BinSegCtx *ctx = BinSegCtx_Make(bf, ZERO);

    Str *one = Str_CstrRef(m, "Value Alpha One First Top Rockin!");
    BinSegCtx_Send(ctx, one);

    Str *two = Str_CstrRef(m, "And here is the description.");
    BinSegCtx_Send(ctx, two);
     
    BinSegCtx_Load(ctx);

    r |= Test((ctx->type.state & END) == END, 
        "Ctx finished with status END", NULL);

    r |= Test(ctx->records->nvalues == 2, 
        "Ctx has two values", NULL);

    r |= Test(Equals(Span_Get(ctx->records, 0), one), 
        "first value is expected", NULL);

    r |= Test(Equals(Span_Get(ctx->records, 1), two), 
        "second value is expected", NULL);

    DebugStack_Pop();
    return r;
}

status BinSegCollection_Tests(MemCh *m){
    DebugStack_Push(NULL, 0);
    status r = READY;
    void *args[5];
    
    Buff *bf = Buff_Make(m, ZERO);
    BinSegCtx *ctx = BinSegCtx_Make(bf, ZERO);

    Str *one = Str_CstrRef(m, "Value Alpha One First Top Rockin!");
    Str *two = Str_CstrRef(m, "Boo Value Two, not as good.");
    Str *three = Str_CstrRef(m, "Maybe Three is a new beginning?");
    Span *p = Span_Make(m);
    Span_Add(p, one);
    Span_Add(p, two);
    Span_Add(p, three);
    BinSegCtx_Send(ctx, p);

    Str *keyA = Str_CstrRef(m, "A");
    Str *apple = Str_CstrRef(m, "Alphabet");
    Str *keyB = Str_CstrRef(m, "B");
    Str *bannana = Str_CstrRef(m, "Baking");
    Str *keyC = Str_CstrRef(m, "C");
    Str *carrot = Str_CstrRef(m, "Casualty");
    Str *keyD = Str_CstrRef(m, "D");
    Str *dice = Str_CstrRef(m, "Delaware");
    Table *tbl = Table_Make(m);
    Table_Set(tbl, keyA, apple);
    Table_Set(tbl, keyB, bannana);
    Table_Set(tbl, keyC, carrot);
    Table_Set(tbl, keyD, dice);
    BinSegCtx_Send(ctx, tbl);

    StrVec *v = StrVec_From(m, Str_FromCstr(m, "one", ZERO));
    StrVec_Add(v, Str_FromCstr(m, "+", ZERO)); 
    StrVec_Add(v, Str_FromCstr(m, "two", ZERO));
    StrVec_Add(v, Str_FromCstr(m, "+", ZERO));
    StrVec_Add(v, Str_FromCstr(m, "three", ZERO));
    StrVec_Add(v, Str_FromCstr(m, "+", ZERO));
    StrVec_Add(v, Str_FromCstr(m, "four", ZERO));
    BinSegCtx_Send(ctx, v);

    BinSegCtx_Load(ctx);

    r |= Test((ctx->type.state & END) == END, 
        "Ctx finished with status END", NULL);

    Span *expP = Span_Get(ctx->records, 0);
    args[0] = expP;
    args[1] = NULL;
    r |= Test(expP->type.of == TYPE_SPAN && expP->nvalues == 3,
        "Ctx records contain a span with 3 values @", args);

    Table *tblP = Span_Get(ctx->records, 1);
    args[0] = tblP;
    args[1] = NULL;
    r |= Test(tblP->type.of == TYPE_TABLE && tblP->nvalues == 4,
        "Ctx records contain a table with 4 values @", args);

    StrVec *vP = Span_Get(ctx->records, 2);
    args[0] = vP;
    args[1] = NULL;
    r |= Test(vP->type.of == TYPE_STRVEC && vP->total == v->total,
        "Ctx records contain a strvec that matches the expected length @", args);

    r |= ERROR;

    DebugStack_Pop();
    return r;
}

status BinSegV_Tests(MemCh *m){
    DebugStack_Push(NULL, 0);
    status r = READY;
    void *args[5];
    
    /*
    Buff *bf = Buff_Make(m, ZERO);
    BinSegCtx *ctx = BinSegCtx_Make(bf, NULL, NULL, BINSEG_VISIBLE);

    Str *one = Str_CstrRef(m, "Value Alpha One First Top Rockin!");
    Str *two = Str_CstrRef(m, "Boo Value Two, not as good.");
    Str *three = Str_CstrRef(m, "Maybe Three is a new beginning?");
    Span *p = Span_Make(m);
    i16 spanId = ctx->func(ctx, NULL);
    i16 tableId = ctx->func(ctx, NULL);
    Span_Add(p, one);
    Span_Add(p, two);
    Span_Add(p, three);
    BinSegCtx_Send(ctx, p, spanId);

    Str *keyA = Str_CstrRef(m, "A");
    Str *apple = Str_CstrRef(m, "Alphabet");
    Str *keyB = Str_CstrRef(m, "B");
    Str *bannana = Str_CstrRef(m, "Baking");
    Str *keyC = Str_CstrRef(m, "C");
    Str *carrot = Str_CstrRef(m, "Casualty");
    Str *keyD = Str_CstrRef(m, "D");
    Str *dice = Str_CstrRef(m, "Delaware");
    Table *tbl = Table_Make(m);
    Table_Set(tbl, keyA, apple);
    Table_Set(tbl, keyB, bannana);
    Table_Set(tbl, keyC, carrot);
    Table_Set(tbl, keyD, dice);
    BinSegCtx_Send(ctx, tbl, tableId);

    ctx->keys = Table_Make(m);
    Str *key1 = Str_CstrRef(m, "list");
    Table_Set(ctx->keys, I16_Wrapped(m, spanId), key1);
    Str *key2 = Str_CstrRef(m, "dict");
    Table_Set(ctx->keys, I16_Wrapped(m, tableId), key2);

    BinSegCtx_Load(ctx);

    r |= Test((ctx->type.state & (SUCCESS|END)) == (SUCCESS|END), 
        "Ctx finished with status SUCCESS|END", NULL);

    Span *list = (Span *)Table_Get(ctx->tbl, key1);
    Str *s = (Str *)Span_Get(list, 0);
    r |= Test(Equals(s, one), "first value is expected", NULL);
    s = (Str *)Span_Get(list, 1);
    r |= Test(Equals(s, two), "second value is expected", NULL);
    s = (Str *)Span_Get(list, 2);
    r |= Test(Equals(s, three), "third value is expected", NULL);

    Table *dict = (Span *)Table_Get(ctx->tbl, key2);
    s = (Str *)Table_Get(dict, keyC);
    args[0] = carrot;
    args[1] = s;
    args[2] = NULL;
    r |= Test(Equals(s, carrot), "keyC value is equal, expected @, have &", args);

    s = (Str *)Table_Get(dict, keyD);
    args[0] = dice;
    args[1] = s;
    args[2] = NULL;
    r |= Test(Equals(s, dice), "keyD value is equal, expected @, have &", args);
    */

    DebugStack_Pop();
    return r;
}

status BinSegReversedV_Tests(MemCh *m){
    DebugStack_Push(NULL, 0);
    status r = READY;
    void *args[5];
    
    /*
    Buff *bf = Buff_Make(m, ZERO);
    BinSegCtx *ctx = BinSegCtx_Make(bf, NULL, NULL, (BINSEG_REVERSED|BINSEG_VISIBLE));

    Str *one = Str_CstrRef(m, "Value Alpha One First Top Rockin!");
    Str *two = Str_CstrRef(m, "Boo Value Two, not as good.");
    Str *three = Str_CstrRef(m, "Maybe Three is a new beginning?");
    Span *p = Span_Make(m);
    i16 spanId = ctx->func(ctx, NULL);
    i16 tableId = ctx->func(ctx, NULL);
    Span_Add(p, one);
    Span_Add(p, two);
    Span_Add(p, three);
    BinSegCtx_Send(ctx, p, spanId);

    Str *keyA = Str_CstrRef(m, "A");
    Str *apple = Str_CstrRef(m, "Alphabet");
    Str *keyB = Str_CstrRef(m, "B");
    Str *bannana = Str_CstrRef(m, "Baking");
    Str *keyC = Str_CstrRef(m, "C");
    Str *carrot = Str_CstrRef(m, "Casualty");
    Str *keyD = Str_CstrRef(m, "D");
    Str *dice = Str_CstrRef(m, "Delaware");
    Table *tbl = Table_Make(m);
    Table_Set(tbl, keyA, apple);
    Table_Set(tbl, keyB, bannana);
    Table_Set(tbl, keyC, carrot);
    Table_Set(tbl, keyD, dice);
    BinSegCtx_Send(ctx, tbl, tableId);

    ctx->keys = Table_Make(m);
    Str *key1 = Str_CstrRef(m, "list");
    Table_Set(ctx->keys, I16_Wrapped(m, spanId), key1);
    Str *key2 = Str_CstrRef(m, "dict");
    Table_Set(ctx->keys, I16_Wrapped(m, tableId), key2);

    BinSegCtx_Load(ctx);

    r |= Test((ctx->type.state & (SUCCESS|END)) == (SUCCESS|END), 
        "Ctx finished with status SUCCESS|END", NULL);

    Span *list = (Span *)Table_Get(ctx->tbl, key1);
    Str *s = (Str *)Span_Get(list, 0);
    r |= Test(Equals(s, one), "first value is expected", NULL);
    s = (Str *)Span_Get(list, 1);
    r |= Test(Equals(s, two), "second value is expected", NULL);
    s = (Str *)Span_Get(list, 2);
    r |= Test(Equals(s, three), "third value is expected", NULL);

    Table *dict = (Span *)Table_Get(ctx->tbl, key2);
    s = (Str *)Table_Get(dict, keyC);
    args[0] = carrot;
    args[1] = s;
    args[2] = NULL;
    r |= Test(Equals(s, carrot), "keyC value is equal, expected @, have &", args);

    s = (Str *)Table_Get(dict, keyD);
    args[0] = dice;
    args[1] = s;
    args[2] = NULL;
    r |= Test(Equals(s, dice),
        "keyD value is equal, expected @, have &", args);
    */

    DebugStack_Pop();
    return r;
}
