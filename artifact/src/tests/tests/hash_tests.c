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
    void *args1[] = {&expected, &expected, &h, &h, NULL};
    r = Test(h == expected, "Expected hash to equal _i8/_b8, found: _i8/_b8", args1);

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

    MemCh_Free(m);
    return r;
}
