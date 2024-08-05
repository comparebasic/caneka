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
    Bits_Print((byte *)&h, sizeof(util), "Hash for string ", COLOR_DARK, TRUE);
    printf("\n");

    int x;
    x = 15;
    Bits_Print((byte *)&x, sizeof(int), "", COLOR_DARK, TRUE);
    printf("\n");
    x = 255;
    Bits_Print((byte *)&x, sizeof(int), "", COLOR_DARK, TRUE);
    printf("\n");
    x = 4095;
    Bits_Print((byte *)&x, sizeof(int), "", COLOR_DARK, TRUE);
    printf("\n");
    x = 65535;
    Bits_Print((byte *)&x, sizeof(int), "", COLOR_DARK, TRUE);
    printf("\n");
    x = 1048575;
    Bits_Print((byte *)&x, sizeof(int), "", COLOR_DARK, TRUE);
    printf("\n");

    MemCtx_Free(m);
    return r;
}
