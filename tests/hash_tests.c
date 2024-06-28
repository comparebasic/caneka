#include <external.h>
#include <caneka.h>
#include <testsuite.h>

status Hash_Tests(MemCtx *gm){
    MemCtx *m = MemCtx_Make();
    String *s;
    status r = TEST_OK;

    s = String_From(m, bytes("Hi"));

    util expected;
    util h;

    h = Get_Hash((Abstract *)s);
    expected = 4291084750259606489;
    Bits_Print((byte *)&h, sizeof(util), "Has for string %ld\n", COLOR_DARK);
    printf("\n");

    s = String_From(m, bytes("Hi"));
    Table_Get(m, NULL, (Abstract *)s);
    s = String_From(m, bytes("Yo Dude!!!"));
    Table_Get(m, NULL, (Abstract *)s);
    s = String_From(m, bytes("Its Hashing"));
    Table_Get(m, NULL, (Abstract *)s);
    s = String_From(m, bytes("Bannana"));
    Table_Get(m, NULL, (Abstract *)s);

    r |= Test(h == expected, 
        "Has of string %s is expected %ld found %ld", s->bytes, expected, h);

    MemCtx_Free(m);
    return r;
}
