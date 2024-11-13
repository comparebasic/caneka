
#include <external.h>
#include <caneka.h>

status MemLocal_Tests(MemCtx *gm){
    status r = READY;
    MemCtx *m = MemCtx_Make();
    MemLocal *ml = MemLocal_Make(m);

    String *s = String_Make(ml->m, bytes("Hidy Ho!"));
    MemLocal_Set(ml, String_Make(ml->m, bytes("keyHi")), (Abstract *)s);

    String *exp = (String *)Table_Get(ml->tbl, (Abstract *)String_Make(m, bytes("keyHi")));
    r |= Test(String_Equals(s, exp), "Retrieved string equals, even though it was saved with local coords, have '%s'", exp->bytes);

    MemCtx_Free(ml->m);
    return r;
}
