#include <external.h>
#include <caneka.h>
#include <test_module.h>
#include "tests.h"

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

    NodeObj *node = Inst_Make(m, TYPE_NODEOBJ); 
    Seel_Set(node, K(m, "name"), Sv(m, "h1"));
    Table *atts = Table_Make(m);
    Table_Set(atts, K(m, "style"), K(m, "font-weight: bold"));
    Seel_Set(node, K(m, "atts"), atts);
    Table *children = Table_Make(m);
    Table_Set(children, I32_Wrapped(m, children->nvalues), K(m, "Fancy Title Here"));
    Seel_Set(node, K(m, "children"), children);
    BinSegCtx_Send(ctx, node);

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

    NodeObj *expNode = Span_Get(ctx->records, 3);
    args[0] = expNode;
    args[1] = NULL;
    r |= Test(expNode->type.of == TYPE_NODEOBJ,
        "Ctx records contain a node obj @", args);

    args[0] = K(m, "font-weight: bold");
    args[1] = Table_Get(Seel_Get(expNode, K(m, "atts")), K(m, "style"));
    args[2] = NULL;
    r |= Test(Equals(args[0], args[1]),
        "Ctx records contain a node with an atts property style that matches @, have @", args);

    DebugStack_Pop();
    return r;
}
