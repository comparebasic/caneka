#include <external.h>
#include <caneka.h>

status MemLocal_Tests(MemCtx *gm){
    status r = READY;
    /*
    MemCtx *m = MemCtx_Make();
    Span *ml = MemLocal_Make();

    String *s = String_Make(ml->m, bytes("Hidy Ho!"));
    Table_Set(ml, String_Make(ml->m, bytes("keyHi")), (Abstract *)s);

    String *key = String_Make(m, bytes("keyHi"));
    String *exp = (String *)Table_Get(ml->tbl, (Abstract *)key);
    r |= Test(String_Equals(s, exp), "Retrieved string equals, even though it was saved with local coords, have '%s'", exp->bytes);

    IoCtx *root = IoCtxTests_GetRootCtx(m);

    String *mlName = String_Make(m, bytes("memlocal"));
    IoCtx *mlctx = IoCtx_Make(m, mlName, NULL, root);
    IoCtx_Persist(m, mlctx);

    MemLocal_Persist(m, ml, mlctx);

    MemLocal *mlLoaded = MemLocal_Load(m, mlctx);

    String *expNew = (String *)Table_Get(mlLoaded->tbl, (Abstract *)key);
    r |= Test(String_Equals(s, expNew), "Retrieved from re-awakened MemLocal, string equals, even though it was saved with local coords, have '%s'", expNew->bytes);

    MemLocal_Destroy(m, mlctx);

    String *onePath = IoCtx_GetMStorePath(m, mlctx);
    char *onePath_cstr = String_ToChars(m, onePath);
    DIR* dir = opendir(onePath_cstr);
    dir = opendir(onePath_cstr);
    r |= Test(dir == NULL, "MemLocal dir destroyed %s", onePath_cstr);

    IoCtx_Destroy(m, mlctx, NULL);

    MemCtx_Free(ml->m);
    */
    return r;
}
