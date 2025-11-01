#include <external.h>
#include <caneka.h>

status Hash_Tests(MemCh *gm){
    MemCh *m = MemCh_Make();
    Str *s;
    status r = SUCCESS;

    s = Str_CstrRef(m, "Hi");

    util expected;
    util h;

    h = Get_Hash((Abstract *)s);
    expected = 4291084750259606489;
    Abstract *args1[] = {
        (Abstract *)Str_Ref(m, (byte *)&expected, sizeof(util), sizeof(util), DEBUG|STRING_BINARY),
        (Abstract *)I64_Wrapped(m, expected),
        (Abstract *)Str_Ref(m, (byte *)&h, sizeof(util), sizeof(util), DEBUG|STRING_BINARY),
        (Abstract *)I64_Wrapped(m, h),
        NULL
    };
    r = Test(h == expected, "Expected hash to equal @/$, found: @/$", args1);

    MemCh_Free(m);
    return r;
}
