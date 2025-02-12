#include <external.h>
#include <caneka.h>

status Crypto_KeyTests(MemCtx *gm){
    DebugStack_Push("Crypto_KeyTests", TYPE_CSTR);
    status r = READY;
    MemCtx *m = MemCtx_Make();

    char buff[PATH_BUFFLEN];
    String *path = String_Make(m, bytes(getcwd(buff, PATH_BUFFLEN)));
    char *cstr = "/dist/fixtures/";
    String_AddBytes(m, path, bytes(cstr), strlen(cstr));
    String *pubPath = String_Clone(m, path);
    cstr = "key.pem";
    String_AddBytes(m, path, bytes(cstr), strlen(cstr));
    cstr = "pub-key.pem";
    String_AddBytes(m, pubPath, bytes(cstr), strlen(cstr));
    
    EcKey *ecKey = EcKey_FromPaths(m, path, pubPath, NULL);
    r |= Test((ecKey->type.state & ERROR) == 0, "EC key has no error flag");

    MemCtx_Free(m);
    DebugStack_Pop();
    return r;
}
