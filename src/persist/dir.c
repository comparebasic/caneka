#include <external.h>
#include <caneka.h>

status Dir_Climb(MemCtx *m, String *path, DoFunc dir, DblFunc file){
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
                dir(m, (Abstract *)s);
                Dir_Climb(m, s, dir, file);
            }else{
                file(m, (Abstract *)path, (Abstract *)String_Make(m, bytes(ent->d_name)));
            }
        }
        closedir(d);
        return SUCCESS;
    }else{
        return ERROR;
    }
}

status Dir_CheckCreate(MemCtx *m, String *path){
    Span *cmd = Span_Make(m, TYPE_SPAN);
    Span_Add(cmd, (Abstract *)String_Make(m, bytes("mkdir")));
    Span_Add(cmd, (Abstract *)String_Make(m, bytes("-p")));
    Span_Add(cmd, (Abstract *)path);

    return SubProcess(m, cmd, path);
}
