
#include <external.h>
#include <caneka.h>

status MemLocal_Tests(MemCtx *gm){
    status r = READY;
    MemCtx *m = MemCtx_Make();
    MemLocal *ml = MemLocal_Make(m);

    String *s = String_Make(ml->m, bytes("Hidy Ho!"));
    MemLocal_Set(ml, String_Make(ml->m, bytes("keyHi")), (Abstract *)s);

    String *key = String_Make(m, bytes("keyHi"));
    printf("\x1b[%dmGetting\x1b[0m\n", COLOR_CYAN);
    String *exp = (String *)Table_Get(ml->tbl, (Abstract *)key);
    printf("before %ld\n", (util)exp);
    r |= Test(String_Equals(s, exp), "Retrieved string equals, even though it was saved with local coords, have '%s'", exp->bytes);

    IoCtx *root = IoCtxTests_GetRootCtx(m);

    IoCtx *mlctx = IoCtx_Make(m, String_Make(m, bytes("memlocal")), NULL, root);
    IoCtx_Persist(m, mlctx);

    MemLocal_Persist(m, ml, mlctx);

    MemLocal *mlLoaded = MemLocal_Load(m, String_Make(m, bytes("memslab")), mlctx);

    String *expNew = (String *)Table_Get(mlLoaded->tbl, (Abstract *)key);
    printf("after %ld\n", (util)expNew);
    r |= Test(String_Equals(s, expNew), "Retrieved from re-awakened MemLocal, string equals, even though it was saved with local coords, have '%s'", expNew->bytes);

    expNew->bytes[0] = 'X';
    printf("%s vs %s\n", exp->bytes, expNew->bytes);

    MemCtx_Free(ml->m);
    return r;
}
