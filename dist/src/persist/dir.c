#include <external.h>
#include <caneka.h>

static rmDir(MemCtx *m, String *path, Abstract *source){
    return unlink(String_ToChars(m, path) == 0 ? SUCCESS : ERROR;
}

static rmFile(MemCtx *m, String *path, String *file, Abstract *source){
    String *fpath = String_Clone(m, path); 
    String_AddBytes(m, fpath, bytes("/"), 1);
    String_Add(m, fpath, file);
    return unlink(String_ToChars(m, fpath) == 0 ? SUCCESS : ERROR;
}

status Dir_Destroy(MemCtx *m, String *path){
    return DirClimb(m, path, rmDir, rmFile, NULL);
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
                r | = file(m, path, String_Make(m, bytes(ent->d_name)), source);
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
    Span *cmd = Span_Make(m, TYPE_SPAN);
    Span_Add(cmd, (Abstract *)String_Make(m, bytes("mkdir")));
    Span_Add(cmd, (Abstract *)String_Make(m, bytes("-p")));
    Span_Add(cmd, (Abstract *)path);

    return SubProcess(m, cmd, NULL);
}
