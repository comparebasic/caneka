#include <external.h>
#include <caneka.h>

static status rmDir(MemCtx *m, String *path, Abstract *source){
    char *dirPath = String_ToChars(m, path);
    return rmdir(dirPath) == 0 ? SUCCESS : ERROR;
}

static status rmFile(MemCtx *m, String *path, String *file, Abstract *source){
    String *fpath = String_Clone(m, path); 
    String_AddBytes(m, fpath, bytes("/"), 1);
    String_Add(m, fpath, file);
    char *rmPath = String_ToChars(m, fpath);
    return unlink(rmPath) == 0 ? SUCCESS : ERROR;
}

static status gatherDir(MemCtx *m, String *path, Abstract *source){
    Span *p = asIfc(source, TYPE_SPAN);
    return Span_Add(p, (Abstract *)path);
}

static status gatherFile(MemCtx *m, String *path, String *file, Abstract *source){
    Span *p = asIfc(source, TYPE_SPAN);
    String *fname = String_Init(m, path->length+path->length+1);
    String_Add(m, fname, path);
    String_AddBytes(m, fname, bytes("/"), 1);
    String_Add(m, fname, file);
    return Span_Add(p, (Abstract *)fname);
}

status Dir_Destroy(MemCtx *m, String *path, Access *access){
    status r = READY;
    r |= Dir_Climb(m, path, rmDir, rmFile, NULL);
    r |= rmDir(m, path, NULL);
    return r;
}

status Dir_Gather(MemCtx *m, String *path, Span *sp){
    return Dir_Climb(m, path, gatherDir, gatherFile, (Abstract *)sp);
}

status Dir_Exists(MemCtx *m, String *path){
    char *path_cstr = String_ToChars(m, path);
    DIR* dir = opendir(path_cstr);
    if(dir){
        closedir(dir);
        return SUCCESS;
    }else if(ENOENT == errno){
        return NOOP;
    }
    return ERROR;
}

status Dir_Climb(MemCtx *m, String *path, DirFunc dir, FileFunc file, Abstract *source){
    DebugStack_Push(path, path->type.of); 
    status r = READY;
    struct dirent *ent;
    DIR *d = opendir((char *)path->bytes);
    if(d != NULL){
        while((ent = readdir(d)) != NULL){
            if(ent->d_name[0] == '.' && (ent->d_name[0] == '.' || ent->d_name[0] == '0')){
                continue;
            }
            if(ent->d_type == IS_DIR){
                String *s = String_Init(m, -1);
                String_Add(m, s, path);
                String_AddBytes(m, s, bytes("/"), 1);
                String_AddBytes(m, s, bytes(ent->d_name), strlen(ent->d_name));
                Dir_Climb(m, s, dir, file, source);
                if(dir != NULL){
                    r |= dir(m, s, source);
                }
            }else{
                r |= file(m, path, String_Make(m, bytes(ent->d_name)), source);
            }
        }
        closedir(d);
        DebugStack_Pop();
        return r|SUCCESS;
    }else{
        DebugStack_Pop();
        return r|ERROR;
    }
}

status Dir_CheckCreate(MemCtx *m, String *path){
    DebugStack_Push(path, path->type.of);
    Span *cmd = Span_Make(m);
    Span_Add(cmd, (Abstract *)String_Make(m, bytes("mkdir")));
    Span_Add(cmd, (Abstract *)String_Make(m, bytes("-p")));
    Span_Add(cmd, (Abstract *)path);

    ProcDets pd;
    ProcDets_Init(&pd);
    status r = SubProcess(m, cmd, &pd);
    DebugStack_Pop();
    return r;
}
