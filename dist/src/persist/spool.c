#include <external.h>
#include <caneka.h>

status Spool_Add(MemCtx *m, String *s, Abstract *a){
    File *spool = (File *)as(a, TYPE_FILE);
    if(spool->data == NULL){
        spool->data = String_Init(m, STRING_EXTEND);
    }
    String_Add(m, spool->data, s);
    spool->type.state |= FILE_UPDATED;
    status r = File_Persist(m, spool);
    if((r & ERROR) == 0){
        String_Reset(spool->data);
    }
    return r;
}

status Spool_Trunc(File *file){
    FILE *f = fopen((char *)file->abs->bytes, "w");
    if(f == NULL){
        return ERROR;
    }
    fclose(f);
    return SUCCESS;
}

File *Spool_Init(File *file, String *path, Access *access, IoCtx *ctx){
    File_Init(file, path, access, ctx);
    file->type.state |= FILE_SPOOL;
    file->abs = file->path;
    return file;
}


File *Spool_Make(MemCtx *m, String *path, Access *access, IoCtx *ctx){
    File *file = File_Make(m, path, access, ctx);
    file->type.state |= FILE_SPOOL;
    return file;
}
