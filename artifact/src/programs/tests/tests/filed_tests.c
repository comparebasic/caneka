#include <external.h>
#include <caneka.h>

status FileD_Tests(MemCh *gm){
    DebugStack_Push(NULL, 0);
    Abstract *args[5];
    status r = READY;
    MemCh *m = MemCh_Make();

    Str *path = IoUtil_GetAbsPath(m, Str_CstrRef(m, "./examples/file.d"));
    unlink(Str_Cstr(m, path));

    File *f = File_Make(m, path, NULL, (STREAM_CREATE|STREAM_TO_FD));
    BinSegCtx *ctx = FileD_Open(f);

    Str *name = Str_CstrRef(m, "Sam-Iam-ICan");
    Str *pw = Str_CstrRef(m, "hI90_jfiu#ud92j$_22uj");
    Str *email = Str_CstrRef(m, "puppies@example.com");
    i16 nameId = ctx->func(ctx, NULL);
    i16 pwId = ctx->func(ctx, NULL);
    i16 emailId = ctx->func(ctx, NULL);
    
    FileD_Add(ctx, nameId, (Abstract *)name);
    FileD_Add(ctx, pwId, (Abstract *)pw);
    FileD_Add(ctx, emailId, (Abstract *)email);

    args[0] = (Abstract *)ctx;
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
