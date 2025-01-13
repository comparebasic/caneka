#include <external.h>
#include <caneka.h>

status IterStr_Tests(MemCtx *gm){
    status r = READY;
    MemCtx *m = MemCtx_Make();
    String *s;

    int source[] = {
         1, 73451,
         2, 73452,
         3, 73453,
         4, 73454,
         5, 73455,
         6, 73456,
         7, 73457,
         8, 73458,
         9, 73459,
         10, 734510,
         11, 734511,
         12, 734512,
         13, 734513,
         14, 734514,
         15, 734515,
         16, 734516,
         17, 734517,
         18, 734518,
         19, 734519,
         20, 734520,
         21, 734521,
         22, 734522,
         23, 734523,
         24, 734524,
         25, 734525,
         26, 734526,
         27, 734527,
         28, 734528,
         29, 734529,
         30, 734530,
         31, 734531,
         32, 734532,
         33, 734533,
         34, 734534,
         35, 734535,
         36, 734536,
         37, 734537,
         38, 734538,
         39, 734539,
         40, 734540,
         41, 734541,
         42, 734542,
         43, 734543,
         44, 734544,
         45, 734545,
         46, 734546,
         47, 734547,
         48, 734548,
         49, 734549,
         50, 734550,
         51, 734551,
         52, 734552,
         53, 734553,
         54, 734554,
         55, 734555,
         56, 734556,
         57, 734557,
         58, 734558,
         59, 734559,
         60, 734560,
         61, 734561,
         62, 734562,
         63, 734563,
         64, 734564,
         65, 734565,
         66, 734566,
         67, 734567,
         68, 734568,
         69, 734569,
         0,0
    };

    s = String_Init(m, sizeof(source));

    IntPair *expected = (IntPair*)source;
    s->type.state |= FLAG_STRING_CONTIGUOUS;
    while(expected->a > 0){
        String_AddBytes(m, s, bytes(expected), sizeof(IntPair));
        expected++;
    }

    expected = (IntPair*)source;
    IterStr it;
    IterStr_Init(&it, s, sizeof(IntPair));
    int i = 0;
    while((IterStr_Next(&it) & END) == 0){
        IntPair *x = (IntPair *)IterStr_Get(&it);
        r |= Test(x->a == expected->a && x->b == expected->b, "Expected (%d) a and b matches, have %d/%d vs %d/%d", i++, x->a, x->b, expected->a, expected->b);
        expected++;
    }

    MemCtx_Free(m);

    r |= SUCCESS;
    return r;
}
