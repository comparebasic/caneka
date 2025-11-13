#include <external.h>
#include <caneka.h>

static status test_ObjKeyAndOrder(MemCh *m, Table *pt, i32 orderIdx, void *key, void *value){
    status r = READY;
    Hashed *h = Span_Get(pt, orderIdx);
    void *args[] = {
        I32_Wrapped(m, h->orderIdx),
        key,
        value,
        h,
        NULL
    };
    return Test(h->value == Table_Get(pt, key) && Equals(h->key, key) && Equals(h->value, value), 
        "Object data lines up with expectations of ordIdx @, key @, value, @, have &", args);
}

static status filterFunc(MemCh *m, void *obj, void *crit){
    Single *sg = (Single *)Table_Get((Table *)obj, crit);
    void *args[] = {sg, NULL};
    if(sg != NULL && sg->val.i > 5){
        Out("^c.filter YES: crit:@^0\n", args);
        return SUCCESS;
    }
    Out("^c.filter NO: crit:@^0\n", args);
    return NOOP; 
}

status Object_Tests(MemCh *gm){
    DebugStack_Push(NULL, 0);
    status r = READY;
    MemCh *m = MemCh_Make();

    Table *pt = Table_Make(m);
    Abstract *h;
    Abstract *key;
    Abstract *value;

    Table_Set(pt, Str_CstrRef(m, "One"), I8_Wrapped(m, 1));
    Table_Set(pt, Str_CstrRef(m, "Two"), I8_Wrapped(m, 2));
    Table_Set(pt, Str_CstrRef(m, "Three"), I8_Wrapped(m, 3));

    r |= test_ObjKeyAndOrder(m, pt, 0, Str_CstrRef(m, "One"), I8_Wrapped(m, 1));
    r |= test_ObjKeyAndOrder(m, pt, 1, Str_CstrRef(m, "Two"), I8_Wrapped(m, 2));
    r |= test_ObjKeyAndOrder(m, pt, 2, Str_CstrRef(m, "Three"), I8_Wrapped(m, 3));

    MemCh_Free(m);
    DebugStack_Pop();
    return r;
}

status ObjectFilter_Tests(MemCh *gm){
    DebugStack_Push(NULL, 0);
    status r = READY;
    MemCh *m = MemCh_Make();
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

    args[0] = obj;
    args[1] = NULL;
    Out("^p.Object &^0\n", args);

    r |= SUCCESS;

    MemCh_Free(m);
    DebugStack_Pop();
    return r;
}
