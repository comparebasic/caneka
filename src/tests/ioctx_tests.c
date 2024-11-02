#include <external.h>
#include <caneka.h>

status IoCtx_Tests(MemCtx *gm){
    status r = READY;
    MemCtx *m = MemCtx_Make();

    char buff[PATH_BUFFLEN];
    char *path = getcwd(buff, PATH_BUFFLEN);
    int l = strlen(path);
    int subL = strlen("tmp");
    int total = l+subL+1;
    if(total > PATH_BUFFLEN){
        Fatal("MakeRoot path too long", TYPE_IOCTX);
    }
    buff[l] = '/';
    memcpy(buff+l+1, "tmp", subL);
    buff[total] = '\0';

    IoCtx *root = IoCtx_Make(m, String_Make(m, bytes(buff)), NULL, NULL);

    IoCtx_Persist(m, root);

    IoCtx *one = IoCtx_Make(m, String_Make(m, bytes("one")), NULL, root);
    IoCtx_Persist(m, one);

    String *onePath = IoCtx_GetPath(m, one, NULL);
    char *onePath_cstr = String_ToChars(m, onePath);
    DIR* dir = opendir(onePath_cstr);
    r |= Test(dir != NULL, "dir exists %s", onePath_cstr);
    IoCtx_Destroy(m, one, NULL);
    dir = opendir(onePath_cstr);
    r |= Test(dir == NULL, "dir destroyed %s", onePath_cstr);

    MemCtx_Free(m);
    return r;
}
