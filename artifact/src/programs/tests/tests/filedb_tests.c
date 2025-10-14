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
    i16 nameId = fdb->ctx->func(fdb->ctx, NULL);
    Str *name = Str_CstrRef(m, "Sam-Iam-ICan");
    FileDB_Add(fdb, nameId, (Abstract *)name);
    FileDB_Close(fdb);

    fdb = FileDB_Make(m, path);
    FileDB_Open(fdb);

    r |= Test(fdb->ctx->latestId == 1, "Latest ID has been set for one record", NULL);

    Table *keys = Table_Make(m);
    Str *key1 = Str_CstrRef(m, "name");
    Table_Set(keys, (Abstract *)I16_Wrapped(m, nameId), (Abstract *)key1);

    Table *tbl = FileDB_ToTbl(fdb, keys);
    Str *s = (Str *)Table_Get(tbl, (Abstract *)key1);

    args[0] = (Abstract *)name;
    args[1] = (Abstract *)s;
    args[2] = NULL;
    r |= Test(Equals((Abstract *)s, (Abstract *)name), 
        "Found name from table, exected, have", args);
    FileDB_Close(fdb);

    FileDB_Open(fdb);
    Str *pw = Str_CstrRef(m, "hI90_jfiu#ud92j$_22uj");
    Str *email = Str_CstrRef(m, "puppies@example.com");
    i16 pwId = fdb->ctx->func(fdb->ctx, NULL);
    i16 emailId = fdb->ctx->func(fdb->ctx, NULL);
    
    FileDB_Add(fdb, pwId, (Abstract *)pw);
    FileDB_Add(fdb, emailId, (Abstract *)email);
    FileDB_Close(fdb);

    keys = Table_Make(m);
    key1 = Str_CstrRef(m, "name");
    Str *key2 = Str_CstrRef(m, "pw");
    Str *key3 = Str_CstrRef(m, "email");
    Table_Set(keys, (Abstract *)I16_Wrapped(m, nameId), (Abstract *)key1);
    Table_Set(keys, (Abstract *)I16_Wrapped(m, pwId),
        (Abstract *)key2);
    Table_Set(keys, (Abstract *)I16_Wrapped(m, emailId),
        (Abstract *)key3);

    tbl = FileDB_ToTbl(fdb, keys);

    s = (Str *)Table_Get(tbl, (Abstract *)key1);
    args[0] = (Abstract *)name;
    args[1] = (Abstract *)s;
    args[2] = NULL;
    r |= Test(Equals((Abstract *)s, (Abstract *)name), 
        "Found name from table, exected $, have $", args);

    s = (Str *)Table_Get(tbl, (Abstract *)key2);
    args[0] = (Abstract *)pw;
    args[1] = (Abstract *)s;
    args[2] = NULL;
    r |= Test(Equals((Abstract *)s, (Abstract *)pw),
        "Found pw from table, exected $, have $", args);

    s = (Str *)Table_Get(tbl, (Abstract *)key3);
    args[0] = (Abstract *)email;
    args[1] = (Abstract *)s;
    args[2] = NULL;
    r |= Test(Equals((Abstract *)s, (Abstract *)email),
        "Found email from table, exected $, have $", args);

    FileDB_Close(fdb);

    FileDB_Open(fdb);
    pw = Str_CstrRef(m, "ruiwoiuer878923jkolwer_#2j");
    FileDB_Add(fdb, pwId, (Abstract *)pw);
    tbl = FileDB_ToTbl(fdb, keys);
    s = (Str *)Table_Get(tbl, (Abstract *)key2);
    args[0] = (Abstract *)pw;
    args[1] = (Abstract *)s;
    args[2] = NULL;
    r |= Test(Equals((Abstract *)s, (Abstract *)pw),
        "Found new pw from table, exected $, have $", args);
    FileDB_Close(fdb);

    MemCh_Free(m);
    DebugStack_Pop();
    return r;
}
