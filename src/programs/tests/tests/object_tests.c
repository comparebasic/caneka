#include <external.h>
#include <caneka.h>

static status test_ObjKeyAndOrder(MemCh *m, Object *pt, i32 orderIdx, Abstract *key, Abstract *value){
    status r = READY;
    Hashed *h = Object_GetByIdx(pt, orderIdx);
    Abstract *args[] = {
        (Abstract *)I32_Wrapped(m, h->orderIdx),
        (Abstract *)key,
        (Abstract *)value,
        (Abstract *)h,
        NULL
    };
    return Test(h->value == Object_Get(pt, key) && Equals(h->key, key) && Equals(h->value, value), 
        "Object data lines up with expectations of ordIdx @, key @, value, @, have &", args);
}

static status filterFunc(MemCh *m, Abstract *obj, Abstract *crit){
    Single *sg = (Single *)Object_Get((Object *)obj, crit);
    Abstract *args[] = {(Abstract *)sg, NULL};
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

    Object *pt = Object_Make(m, ZERO);
    Abstract *h;
    Abstract *key;
    Abstract *value;

    Object_Set(pt, (Abstract *)Str_CstrRef(m, "One"), (Abstract *)I8_Wrapped(m, 1));
    Object_Set(pt, (Abstract *)Str_CstrRef(m, "Two"), (Abstract *)I8_Wrapped(m, 2));
    Object_Set(pt, (Abstract *)Str_CstrRef(m, "Three"), (Abstract *)I8_Wrapped(m, 3));

    r |= test_ObjKeyAndOrder(m, pt, 0, (Abstract *)Str_CstrRef(m, "One"), (Abstract *)I8_Wrapped(m, 1));
    r |= test_ObjKeyAndOrder(m, pt, 1, (Abstract *)Str_CstrRef(m, "Two"), (Abstract *)I8_Wrapped(m, 2));
    r |= test_ObjKeyAndOrder(m, pt, 2, (Abstract *)Str_CstrRef(m, "Three"), (Abstract *)I8_Wrapped(m, 3));

    MemCh_Free(m);
    DebugStack_Pop();
    return r;
}

status ObjectFilter_Tests(MemCh *gm){
    DebugStack_Push(NULL, 0);
    status r = READY;
    MemCh *m = MemCh_Make();
    Abstract *args[5];

    Object *obj = Object_Make(m, TYPE_OBJECT);

    Str *key = Str_FromCstr(m, "val", ZERO);
    StrVec *path = StrVec_From(m, Str_FromCstr(m, "apple.bat.carrot", ZERO));
    Path_DotAnnotate(m, path);
    Object *suObj = Object_Make(m, TYPE_OBJECT);
    Object_Set(suObj, (Abstract*)key, (Abstract *)I32_Wrapped(m, 3));
    Object_ByPath(obj, path, (Abstract *)suObj, SPAN_OP_SET);

    path = StrVec_From(m, Str_FromCstr(m, "apple.beetle", ZERO));
    Path_DotAnnotate(m, path);
    suObj = Object_Make(m, TYPE_OBJECT);
    Object_Set(suObj, (Abstract*)key, (Abstract *)I32_Wrapped(m, 7));
    Object_ByPath(obj, path, (Abstract *)suObj, SPAN_OP_SET);

    path = StrVec_From(m, Str_FromCstr(m, "apple.bat.cinnomon", ZERO));
    Path_DotAnnotate(m, path);
    suObj = Object_Make(m, TYPE_OBJECT);
    Object_Set(suObj, (Abstract*)key, (Abstract *)I32_Wrapped(m, 9));
    Object_ByPath(obj, path, (Abstract *)suObj, SPAN_OP_SET);

    args[0] = (Abstract *)obj;
    args[1] = NULL;
    Out("^p.Object &^0\n", args);

    Object_Filter(obj, filterFunc, (Abstract *)key);

    r |= SUCCESS;

    MemCh_Free(m);
    DebugStack_Pop();
    return r;
}
