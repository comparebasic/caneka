#include <external.h>
#include <caneka.h>

status File_Exists(String *path){
    struct stat st;
    int r = 0;

    r = stat((char *)path->bytes, &st);
    if(r != 0){
        return NOOP;
    }

    return SUCCESS;
}

status File_Unlink(String *path){
    if(unlink((char *)path->bytes) == 0){
        return SUCCESS;
    }
    return ERROR;
}

String *File_GetCwdPath(MemCtx *m, String *path){
    char buff[PATH_BUFFLEN];
    char *pathCstr = getcwd(buff, PATH_BUFFLEN);
    String *s = String_Init(m, STRING_EXTEND);
    String_AddBytes(m, s, bytes(pathCstr), strlen(pathCstr));
    String_AddBytes(m, s, bytes("/"), 1);
    String_Add(m, s, path);
    return s;
}

String *File_GetAbsPath(MemCtx *m, String *path){
    if(path != NULL && path->bytes[0] != '/'){
        return File_GetCwdPath(m, path);
    }
    return path;
}

boolean File_CmpUpdated(MemCtx *m, String *a, String *b, Access *ac){
    DebugStack_Push(a, a->type.of);
    struct stat source_stat;
    struct stat build_stat;
    int r = 0;

    r = stat(String_ToChars(m, a), &source_stat);
    if(r != 0){
        Fatal("Source not found", TYPE_FILE);
        exit(1);
    }
    r = stat(String_ToChars(m, b), &build_stat);
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

