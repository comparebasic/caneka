#include <external.h>
#include <caneka.h>
#include <test_module.h>

static status test_ObjKeyAndOrder(MemCh *m, 
        Table *tbl, Span *ord, i32 orderIdx, void *key, void *value){
    void *args[5];
    status r = READY;
    Hashed *h = Span_Get(ord, orderIdx);
    args[0] = h;
    args[1] = key;
    args[2] = NULL;
    r |= Test(h != NULL, "Hashed found for @/$", args);

    args[0] = I32_Wrapped(m, h->orderIdx);
    args[1] = key;
    args[2] = value;
    args[3] = h;
    args[4] = NULL;
    return Test(h->value == Table_Get(tbl, key) && Equals(h->key, key) && Equals(h->value, value), 
        "Object data lines up with expectations of ordIdx @, key @, value, @, have &", args);
}

static status filterFunc(MemCh *m, void *obj, void *crit){
    Single *sg = (Single *)Table_Get((Table *)obj, crit);
    void *args[] = {sg, NULL};
    if(sg != NULL && sg->val.i > 5){
        return SUCCESS;
    }
    return NOOP; 
}

status Object_Tests(MemCh *m){
    DebugStack_Push(NULL, 0);
    status r = READY;

    Table *pt = Table_Make(m);
    Abstract *h;
    Abstract *key;
    Abstract *value;

    Table_Set(pt, Str_CstrRef(m, "One"), I8_Wrapped(m, 1));
    Table_Set(pt, Str_CstrRef(m, "Two"), I8_Wrapped(m, 2));
    Table_Set(pt, Str_CstrRef(m, "Three"), I8_Wrapped(m, 3));

    Span *ord = Table_Ordered(m, pt);
    void *args[] = {ord, NULL};

    r |= test_ObjKeyAndOrder(m, pt, ord, 0, Str_CstrRef(m, "One"), I8_Wrapped(m, 1));
    r |= test_ObjKeyAndOrder(m, pt, ord, 1, Str_CstrRef(m, "Two"), I8_Wrapped(m, 2));
    r |= test_ObjKeyAndOrder(m, pt, ord, 2, Str_CstrRef(m, "Three"), I8_Wrapped(m, 3));

    DebugStack_Pop();
    return r;
}

status ObjectFilter_Tests(MemCh *m){
    DebugStack_Push(NULL, 0);
    status r = READY;
    void *args[5];

    Table *obj = Table_Make(m);

    Str *key = Str_FromCstr(m, "val", ZERO);
    StrVec *path = StrVec_From(m, Str_FromCstr(m, "apple.bat.carrot", ZERO));
    Path_DotAnnotate(m, path);
    Table *suObj = Table_Make(m);
    Table_Set(suObj, key, I32_Wrapped(m, 3));
    Table_ByPath(obj, path, suObj, SPAN_OP_SET);

    path = StrVec_From(m, Str_FromCstr(m, "apple.beetle", ZERO));
    Path_DotAnnotate(m, path);
    suObj = Table_Make(m);
    Table_Set(suObj, key, I32_Wrapped(m, 7));
    Table_ByPath(obj, path, suObj, SPAN_OP_SET);

    path = StrVec_From(m, Str_FromCstr(m, "apple.bat.cinnomon", ZERO));
    Path_DotAnnotate(m, path);
    suObj = Table_Make(m);
    Table_Set(suObj, key, I32_Wrapped(m, 9));
    Table_ByPath(obj, path, suObj, SPAN_OP_SET);

    r |= SUCCESS;

    DebugStack_Pop();
    return r;
}
