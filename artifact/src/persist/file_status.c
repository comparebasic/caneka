#include <external.h>
#include <caneka.h>

status File_Exists(Str *path){
    struct stat st;
    int r = 0;

    r = stat((char *)path->bytes, &st);
    if(r != 0){
        return NOOP;
    }

    return SUCCESS;
}

status File_Unlink(Str *path){
    if(unlink((char *)path->bytes) == 0){
        return SUCCESS;
    }
    return ERROR;
}

Str *File_GetCwdPath(MemCtx *m, Str *path){
    char buff[STR_DEFAULT];
    char *cstr = getcwd(buff, STR_DEFAULT);
    i64 length = strlen(cstr);
    Str *s = Str_Make(m, length+1);
    return Str_Add(s, bytes(pathCstr), length);
}

Str *File_GetAbsPath(MemCtx *m, Str *path){
    if(path != NULL && path->bytes[0] != '/'){
        return File_GetCwdPath(m, path);
    }
    return path;
}

boolean File_CmpUpdated(MemCtx *m, Str *a, Str *b, Access *ac){
    DebugStack_Push(a, a->type.of);
    struct stat source_stat;
    struct stat build_stat;
    int r = 0;

    char *path_cstr = Str_ToChars(m, a);
    r = stat(path_cstr, &source_stat);
    if(r != 0){
        printf("%s\n", path_cstr);
        Fatal("Source not found", TYPE_FILE);
        exit(1);
    }
    r = stat(Str_ToChars(m, b), &build_stat);
    time_t build_mtime = 0;
    if(r == 0){
        build_mtime = build_stat.st_mtime;
    }

    if(source_stat.st_mtime > build_mtime){
        DebugStack_Pop();
        return TRUE;
    }else{
        DebugStack_Pop();
        return FALSE;
    }
}

