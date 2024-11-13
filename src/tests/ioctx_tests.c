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

    IoCtx *root = IoCtxTests_GetRootCtx(m);

    IoCtx_Persist(m, root);

    IoCtx *one = IoCtx_Make(m, String_Make(m, bytes("one")), NULL, root);
    IoCtx_Persist(m, one);

    String *onePath = IoCtx_GetPath(m, one, NULL);
    char *onePath_cstr = String_ToChars(m, onePath);
    DIR* dir = opendir(onePath_cstr);
    r |= Test(dir != NULL, "dir exists %s", onePath_cstr);

    String *fname = String_Make(m, bytes("file.one"));
    File *file = File_Make(m, fname, NULL, one);
    char *cstr = "Test content here is a thing\n";
    file->data = String_Make(m, bytes(cstr));
    file->type.state |= (FILE_UPDATED|FILE_TRACKED);
    File_Persist(m, file);

    Debug_Print((void *)one->files, 0, "Files: ",COLOR_PURPLE, TRUE);
    printf("\n");

    FILE *f = fopen((char *)file->abs->bytes, "r");
    memset(buff, 0, sizeof(buff));
    l = fread(buff, 1, sizeof(buff), f);
    r |= Test(l == file->data->length, "file length match, have %ld", l);
    r |= Test(String_EqualsBytes(file->data, bytes(buff)), "String content matches, have: '%s'", buff);

    IoCtx_Destroy(m, one, NULL);
    dir = opendir(onePath_cstr);
    r |= Test(dir == NULL, "dir destroyed %s", onePath_cstr);

    MemCtx_Free(m);
    return r;
}
