#include <external.h>
#include <caneka.h>

status Cash_Tests(MemCh *gm){
    DebugStack_Push(NULL, 0);
    status r = READY;
    MemCh *m = MemCh_Make();
    Str *s = NULL; 

    Str *path = File_GetAbsPath(m, Str_CstrRef(m, "./docs/html/header.html"));
    File *f = File_Make(m, path, NULL, STREAM_STRVEC);
    File_Open(f);
    File_Read(f, FILE_READ_MAX);
    File_Close(f);

    Cursor *curs = File_GetCurs(f);
    CashCtx *ctx = CashCtx_FromCurs(m, curs, NULL);
    
    r |= Test(ctx->type.state & SUCCESS,
            "Roebling finished with state SUCCESS for Cash with keys", 
        NULL);

    MemCh_Free(m);
    DebugStack_Pop();
    return r;
}

status CashTempl_Tests(MemCh *gm){
    DebugStack_Push(NULL, 0);
    status r = READY;
    MemCh *m = MemCh_Make();
    Str *s = NULL; 

    Str *path = File_GetAbsPath(m, Str_CstrRef(m, "./docs/html/nav.html"));
    File *f = File_Make(m, path, NULL, STREAM_STRVEC);
    File_Open(f);
    File_Read(f, FILE_READ_MAX);
    File_Close(f);

    Cursor *curs = File_GetCurs(f);
    CashCtx *ctx = CashCtx_FromCurs(m, curs, NULL);
    
    r |= Test(ctx->type.state & SUCCESS,
            "Roebling finished with state SUCCESS for Cash with template logic", 
        NULL);

    MemCh_Free(m);
    DebugStack_Pop();
    return r;
}
