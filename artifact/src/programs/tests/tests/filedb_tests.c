#include <external.h>
#include <caneka.h>

status FileDB_Tests(MemCh *gm){
    DebugStack_Push(NULL, 0);
    Abstract *args[5];
    status r = READY;
    MemCh *m = MemCh_Make();

    Str *path = IoUtil_GetAbsPath(m, Str_CstrRef(m, "./examples/file.d"));
    unlink(Str_Cstr(m, path));

    FileDB *fdb = FileDB_Make(m, path);
    FileDB_Open(fdb);
    fdb->ctx->type.state |= DEBUG;

    i16 nameId = fdb->ctx->func(fdb->ctx, NULL);
    Str *name = Str_CstrRef(m, "Sam-Iam-ICan");
    FileDB_Add(fdb, nameId, (Abstract *)name);
    FileDB_Close(fdb);

    fdb = FileDB_Make(m, path);
    FileDB_Open(fdb);

    r |= Test(fdb->ctx->latestId == 1, "Latest ID has been set for one record", NULL);

    /*
    Str *pw = Str_CstrRef(m, "hI90_jfiu#ud92j$_22uj");
    Str *email = Str_CstrRef(m, "puppies@example.com");
    i16 nameId = fdb->ctx->func(fdb->ctx, NULL);
    i16 pwId = fdb->ctx->func(fdb->ctx, NULL);
    i16 emailId = fdb->ctx->func(fdb->ctx, NULL);
    
    FileDB_Add(fdb, nameId, (Abstract *)name);
    FileDB_Add(fdb, pwId, (Abstract *)pw);
    FileDB_Add(fdb, emailId, (Abstract *)email);

    args[0] = (Abstract *)fdb->ctx;
    args[1] = NULL;
    Out("^p.BinSegCtx &\n", args);

    Str *key1 = Str_CstrRef(m, "name");
    Table *keys = Table_Make(m);
    Table_Set(keys, (Abstract *)I16_Wrapped(m, nameId),
        (Abstract *)key1);
    Str *key2 = Str_CstrRef(m, "pw");
    Table_Set(keys, (Abstract *)I16_Wrapped(m, pwId),
        (Abstract *)key2);
    Str *key3 = Str_CstrRef(m, "email");
    Table_Set(keys, (Abstract *)I16_Wrapped(m, emailId),
        (Abstract *)key3);
    */

    /*
    Table *tbl = FileD_ToTbl(ctx, keys);

    args[0] = (Abstract *)tbl;
    args[1] = NULL;
    Out("^p.Table &\n", args);
    */

    MemCh_Free(m);
    DebugStack_Pop();
    return r;
}
