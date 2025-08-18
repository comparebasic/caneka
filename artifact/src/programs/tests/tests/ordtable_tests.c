#include <external.h>
#include <caneka.h>

static status test_OrdRecord(MemCh *m, PathTable *pt, i32 orderIdx, Abstract *key, Abstract *value){
    status r = READY;
    Hashed *h = PathTable_GetByIdx(pt, orderIdx);
    Abstract *args[] = {
        (Abstract *)I32_Wrapped(m, h->orderIdx),
        (Abstract *)key,
        (Abstract *)value,
        (Abstract *)h,
        NULL
    };
    return Test(h == PathTable_Get(pt, key) && Equals(h->key, key) && Equals(h->value, value), 
        "PathTable data lines up with expectations of ordIdx @, key @, value, @, have &", args);
}

status PathTable_Tests(MemCh *gm){
    DebugStack_Push(NULL, 0);
    status r = READY;
    MemCh *m = MemCh_Make();

    PathTable *pt = PathTable_Make(m);
    Abstract *h;
    Abstract *key;
    Abstract *value;

    PathTable_Set(pt, (Abstract *)Str_CstrRef(m, "One"), (Abstract *)I8_Wrapped(m, 1));
    PathTable_Set(pt, (Abstract *)Str_CstrRef(m, "Two"), (Abstract *)I8_Wrapped(m, 2));
    PathTable_Set(pt, (Abstract *)Str_CstrRef(m, "Three"), (Abstract *)I8_Wrapped(m, 3));

    r |= test_OrdRecord(m, pt, 0, (Abstract *)Str_CstrRef(m, "One"), (Abstract *)I8_Wrapped(m, 1));
    r |= test_OrdRecord(m, pt, 1, (Abstract *)Str_CstrRef(m, "Two"), (Abstract *)I8_Wrapped(m, 2));
    r |= test_OrdRecord(m, pt, 2, (Abstract *)Str_CstrRef(m, "Three"), (Abstract *)I8_Wrapped(m, 3));

    MemCh_Free(m);
    DebugStack_Pop();
    return r;
}
