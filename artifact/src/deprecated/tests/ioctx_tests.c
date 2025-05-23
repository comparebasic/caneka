#include <external.h>
#include <caneka.h>

status IoCtx_Tests(MemCtx *gm){
    DebugStack_Push("IoCtx_Tests", TYPE_CSTR);
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

    String *name = String_Make(m, bytes("one"));
    IoCtx *one = IoCtx_Make(m, name, NULL, root);
    IoCtx_LoadOrReserve(m, one);

    String *onePath = IoCtx_GetPath(m, one, NULL);
    char *onePath_cstr = String_ToChars(m, onePath);

    String *fname = String_Make(m, bytes("file.one"));
    File *file = File_Make(m, fname, NULL, one);
    char *cstr = "Test content here is a thing\n";
    file->data = String_Make(m, bytes(cstr));
    file->type.state |= (FILE_UPDATED|FILE_TRACKED);
    File_Persist(m, file);

    FILE *f = fopen((char *)file->abs->bytes, "r");
    memset(buff, 0, sizeof(buff));
    l = fread(buff, 1, sizeof(buff), f);
    r |= Test(l == file->data->length, "file length match, have %ld", l);
    r |= Test(String_EqualsBytes(file->data, bytes(buff)), "String content matches, have: '%s'", buff);

    String *key = String_Make(one->mstore->m, bytes("key"));
    String *value = String_Make(one->mstore->m, bytes("value"));
    Table_Set(one->mstore, (Abstract *)key, (Abstract *)value);
    IoCtx_Persist(m, one);

    IoCtx oneLoaded;
    String *nameToLoad = String_Make(m, bytes("one"));
    IoCtx_Open(m, &oneLoaded, nameToLoad, NULL, root);

    String *mlValue = (String *)Table_Get(oneLoaded.mstore, (Abstract *)String_Make(m, bytes("key")));
    File *fileOne = (File *)Table_Get(oneLoaded.files, (Abstract *)fname);

    r |= Test(Ifc_Match(mlValue->type.of, TYPE_STRING), "MemLocal value from ctx matches type, have '%s'", Class_ToString(mlValue->type.of));
    r |= Test(String_EqualsBytes(mlValue, bytes("value")), "MemLocal value from ctx has expected bytes , have '%s'", mlValue->bytes);
    r |= Test(Ifc_Match(fileOne->type.of, TYPE_FILE), "File value from ctx matches type, have '%s'", Class_ToString(fileOne->type.of));
    r |= Test(String_EqualsBytes(fileOne->path, bytes("file.one")), "File value from ctx has expected bytes in path , have '%s'", fileOne->path->bytes);

    IoCtx_Destroy(m, one, NULL);
    DIR *dir = opendir(onePath_cstr);
    r |= Test(dir == NULL, "dir destroyed %s", onePath_cstr);

    MemCtx_Free(m);
    DebugStack_Pop();
    return r;
}
