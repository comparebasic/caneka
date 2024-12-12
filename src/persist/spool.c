#include <external.h>
#include <caneka.h>

status Spool_Add(MemCtx *m, String *s, Abstract *a){
    File *spool = (File *)as(a, TYPE_FILE);
    String_Add(m, spool->data, s);
    spool->type.state |= FILE_UPDATED;
    if(String_Length(spool->data) > SPOOL_STRING_COUNT){
        if((File_Persist(m, spool) & SUCCESS) != 0){
            String *s = spool->data;
            while(s != NULL){
                s->length = 0;
                s = String_Next(s);
            }
        }
        return SUCCESS;
    }
    return NOOP;
}

File *Spool_Init(File *file, String *path, Access *access, IoCtx *ctx){
    File_Init(file, path, access, ctx);
    file->type.state |= FILE_SPOOL;
    return file;
}


File *Spool_Make(MemCtx *m, String *path, Access *access, IoCtx *ctx){
    File *file = File_Make(file, path, access, ctx);
    file->type.state |= FILE_SPOOL;
    return file;
}
