#include <external.h>
#include <caneka.h>

static Str _globalS;
static byte _buff[STR_DEFAULT];

Span *FilePathSep = NULL;

static status fnameStr(MemCh *m, Str *s, Str *path, Str *file){
    Str_Add(s, path->bytes, path->length);
    if(path->length > 0 && path->bytes[path->length-1] != '/'){
        Str_Add(s, (byte *)"/", 1);
    }
    Str_Add(s, file->bytes, file->length);
    return SUCCESS;
}

static status rmDir(MemCh *m, Str *path, void *source){
    char *dirPath = Str_Cstr(m, path);
    return rmdir(dirPath) == 0 ? SUCCESS : ERROR;
}

static status rmFile(MemCh *m, Str *path, Str *file, void *source){
    Str *s = &_globalS;
    Str_Init(s, _buff, STR_DEFAULT, STR_DEFAULT);
    fnameStr(m, s, path, file);
    char *rmPath = Str_Cstr(m, s);
    return unlink(rmPath) == 0 ? SUCCESS : ERROR;
}

static status gatherDir(MemCh *m, Str *path, void *source){
    Span *p = (Span *)asIfc(source, TYPE_SPAN);
    StrVec *v = StrVec_From(m, path);
    v->type.state |= MORE;
    return Span_Add(p, v);
}

static status gatherFile(MemCh *m, Str *path, Str *file, void *source){
    Span *p = (Span *)asIfc(source, TYPE_SPAN);
    StrVec *v = StrVec_Make(m);
    StrVec_Add(v, path);
    if(path->bytes[path->length-1] != '/'){
        StrVec_Add(v, Str_Ref(m, (byte *)"/", 1, 1, 0));
    }
    StrVec_Add(v, file);
    return Span_Add(p, v);
}

status Dir_Rm(MemCh *m, Str *path){
    char *dirPath = Str_Cstr(m, path);
    return rmdir(dirPath) == 0 ? SUCCESS : ERROR;
}

status Dir_Destroy(MemCh *m, Str *path){
    status r = READY;
    r |= Dir_Climb(m, path, rmDir, rmFile, NULL);
    r |= rmDir(m, path, NULL);
    return r;
}

status Dir_Gather(MemCh *m, Str *path, Span *sp){
    return Dir_Climb(m, path, gatherDir, gatherFile, sp);
}

status Dir_Exists(MemCh *m, Str *path){
    char *path_cstr = Str_Cstr(m, path);
    DIR* dir = opendir(path_cstr);
    if(dir){
        closedir(dir);
        return SUCCESS;
    }else if(ENOENT == errno){
        return NOOP;
    }
    return ERROR;
}

status Dir_Climb(MemCh *m, Str *path, DirFunc dir, FileFunc file, void *source){
    DebugStack_Push(path, path->type.of); 
    status r = READY;
    struct dirent *ent;
    DIR *d = opendir((char *)path->bytes);
    if(d != NULL){
        while((ent = readdir(d)) != NULL){
            if(ent->d_name[0] == '.' && (ent->d_name[0] == '.' || ent->d_name[0] == '0')){
                continue;
            }
            i64 len = strlen(ent->d_name);
            Str *e = Str_Ref(m, (byte *)ent->d_name, len, len+1, STRING_COPY);
            if(ent->d_type == IS_DIR){
                Str *s = Str_Make(m, STR_DEFAULT);
                fnameStr(m, s, path, e);
                if(dir != NULL){
                    r |= dir(m, s, source);
                }
                Dir_Climb(m, s, dir, file, source);
            }else{
                r |= file(m, path, e, source);
            }
        }
        closedir(d);
        DebugStack_Pop();
        return r|SUCCESS;
    }else{
        void *args[] = {path, NULL};
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "Unable to open Direcotry: @", args);
        DebugStack_Pop();
        return r|ERROR;
    }
}

status Dir_Mk(MemCh *m, Str *path){
    if(mkdir(Str_Cstr(m, path), 0766) == 0){
        return SUCCESS;
    }else{
        return ERROR;
    }
}

status Dir_CheckCreate(MemCh *m, Str *path){
    DebugStack_Push(path, path->type.of);
    Span *cmd = Span_Make(m);
    char *cstr = "mkdir";
    i64 len = strlen(cstr);
    Span_Add(cmd, Str_Ref(m, (byte *)cstr, len, len+1, 0));
    cstr = "-p";
    len = strlen(cstr);
    Span_Add(cmd, Str_Ref(m, (byte *)cstr, len, len+1, 0));
    Span_Add(cmd, path);

    ProcDets pd;
    ProcDets_Init(&pd);
    if(path->type.state & DEBUG){
        cmd->type.state |= DEBUG;
    }
    status r = SubProcess(m, cmd, &pd);
    DebugStack_Pop();
    return r;
}
