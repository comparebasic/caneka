#include <external.h>
#include <caneka.h>

static status test_OrdRecord(MemCh *m, OrdTable *otbl, i32 orderIdx, Abstract *key, Abstract *value){
    status r = READY;
    Hashed *h = OrdTable_GetByIdx(otbl, orderIdx);
    Abstract *args[] = {
        (Abstract *)I32_Wrapped(m, h->orderIdx),
        (Abstract *)key,
        (Abstract *)value,
        (Abstract *)h,
        NULL
    };
    return Test(h == OrdTable_Get(otbl, key) && Equals(h->key, key) && Equals(h->value, value), 
        "OrdTable data lines up with expectations of ordIdx @, key @, value, @, have &", args);
}

status OrdTable_Tests(MemCh *gm){
    DebugStack_Push(NULL, 0);
    status r = READY;
    MemCh *m = MemCh_Make();

    OrdTable *otbl = OrdTable_Make(m);
    Abstract *h;
    Abstract *key;
    Abstract *value;

    OrdTable_Set(otbl, (Abstract *)Str_CstrRef(m, "One"), (Abstract *)I8_Wrapped(m, 1));
    OrdTable_Set(otbl, (Abstract *)Str_CstrRef(m, "Two"), (Abstract *)I8_Wrapped(m, 2));
    OrdTable_Set(otbl, (Abstract *)Str_CstrRef(m, "Three"), (Abstract *)I8_Wrapped(m, 3));

    r |= test_OrdRecord(m, otbl, 0, (Abstract *)Str_CstrRef(m, "One"), (Abstract *)I8_Wrapped(m, 1));
    r |= test_OrdRecord(m, otbl, 1, (Abstract *)Str_CstrRef(m, "Two"), (Abstract *)I8_Wrapped(m, 2));
    r |= test_OrdRecord(m, otbl, 2, (Abstract *)Str_CstrRef(m, "Three"), (Abstract *)I8_Wrapped(m, 3));

    MemCh_Free(m);
    DebugStack_Pop();
    return r;
}
