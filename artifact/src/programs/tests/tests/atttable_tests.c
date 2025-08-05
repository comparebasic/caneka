#include <external.h>
#include <caneka.h>

status AttTable_Tests(MemCh *gm){
    DebugStack_Push(NULL, 0);
    status r = READY;

    MemCh *m = MemCh_Make();

    Str *theKey = Str_CstrRef(m, "theKey");
    Hashed *h = Hashed_Make(m, (Abstract *)theKey);
    h->value = (Abstract *)Str_CstrRef(m, "TheValueOfAllWonderfulThings!!!");
    h->idx = 3;
    h->orderIdx = 7;

    Str *key = Str_CstrRef(m, "id");
    i16 offset = AttTable_Offset((Abstract *)h, (Abstract *)key);

    r |= Test(offset == 16, "id is 16 bytes into the Hashed object", NULL);

    void *att = NULL;
    Single *sg = NULL;
    i32 idx = -1;
    AttTable_Att(m, (Abstract *)h, (Abstract *)key, (Abstract **)&sg, &idx);
    Abstract *args1[] = {
        (Abstract *)sg,
        NULL
    };
    r |= Test(sg->type.of == TYPE_WRAPPED_UTIL && sg->val.value == h->id,
        "id attribute found, have &", args1);

    key = Str_CstrRef(m, "idx");
    AttTable_Att(m, (Abstract *)h, (Abstract *)key, (Abstract **)&sg, &idx);
    Abstract *args2[] = {
        (Abstract *)sg,
        NULL
    };
    r |= Test(sg->type.of == TYPE_WRAPPED_I32 && sg->val.i == h->idx,
        "idx attribute found, have &", args2);

    key = Str_CstrRef(m, "key");
    Str *s = NULL;
    AttTable_Att(m, (Abstract *)h, (Abstract *)key, (Abstract **)&s, &idx);

    Abstract *args3[] = {
        (Abstract *)s,
        (Abstract *)h,
        NULL
    };
    r |= Test(s == theKey,
        "key attribute found, have @ from @", args3);

    key = Str_CstrRef(m, "value");
    AttTable_Att(m, (Abstract *)h, (Abstract *)key, (Abstract **)&s, &idx);

    Abstract *args4[] = {
        (Abstract *)s,
        (Abstract *)h,
        NULL
    };
    r |= Test(s == (Str *)h->value,
        "value attribute found, have @ from @", args4);

    MemCh_Free(m);
    DebugStack_Pop();
    return r;
}
