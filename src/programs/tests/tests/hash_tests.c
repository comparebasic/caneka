#include <external.h>
#include <caneka.h>

status Hash_Tests(MemCh *m){
    Str *s;
    status r = SUCCESS;

    s = Str_CstrRef(m, "Hi");

    util expected;
    util h;

    h = Get_Hash(s);
    expected = 4291084750259606489;
    void *args1[] = {
        Str_Ref(m, (byte *)&expected, sizeof(util), sizeof(util), DEBUG|STRING_BINARY),
        I64_Wrapped(m, expected),
        Str_Ref(m, (byte *)&h, sizeof(util), sizeof(util), DEBUG|STRING_BINARY),
        I64_Wrapped(m, h),
        NULL
    };
    r = Test(h == expected, "Expected hash to equal @/$, found: @/$", args1);

    return r;
}
