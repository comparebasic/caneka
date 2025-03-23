#include <external.h>
#include <caneka.h>

status Hash_Tests(MemCtx *gm){
    MemCtx *m = MemCtx_Make();
    String *s;
    status r = SUCCESS;

    s = String_Make(m, bytes("Hi"));

    util expected;
    util h;

    h = Get_Hash((Abstract *)s);
    expected = 4291084750259606489;
    r = Test(h == expected, "Expected hash to equal %ld, found: %ld", expected, h);

    int x;
    /*
    x = 15;
    printf("%d ", x);
    Bits_Print((byte *)&x, sizeof(int), "", COLOR_DARK, TRUE);
    printf("\n");
    x = 255;
    printf("%d ", x);
    Bits_Print((byte *)&x, sizeof(int), "", COLOR_DARK, TRUE);
    printf("\n");
    x = 4095;
    printf("%d ", x);
    Bits_Print((byte *)&x, sizeof(int), "", COLOR_DARK, TRUE);
    printf("\n");
    x = 65535;
    printf("%d ", x);
    Bits_Print((byte *)&x, sizeof(int), "", COLOR_DARK, TRUE);
    printf("\n");
    x = 1048575;
    printf("%d ", x);
    Bits_Print((byte *)&x, sizeof(int), "", COLOR_DARK, TRUE);
    printf("\n");
    */

    MemCtx_Free(m);
    return r;
}
