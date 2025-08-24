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
