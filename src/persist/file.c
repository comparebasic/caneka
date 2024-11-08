#include <external.h>
#include <caneka.h>

status File_Persist(MemCtx *m, File *file){
    char buff[PATH_BUFFLEN];

    if(file->data != NULL){
        if((file->type.state & FILE_UPDATED) != 0){
            FILE *f = fopen((char *)file->abs->bytes, "w");
            if(f == NULL){
                return ERROR;
            }
            String *s = file->data;
            i64 total = 0;
            while(s != NULL){
                size_t l = fwrite(s->bytes, 1, s->length, f);
                if(l != s->length){
                    return ERROR;
                }
                total += l;
                s = String_Next(s);
            }
            file->type.state &= ~FILE_UPDATED;
            fclose(f);
            if((file->type.state & FILE_TRACKED) != 0){
                int suffixL = strlen(".notes");
                memcpy(buff, file->abs->bytes, file->abs->length);
                memcpy(buff+file->abs->length, ".notes", suffixL);
                buff[file->abs->length+suffixL] = '\0';
                f = fopen(buff, "a");
                if(f == NULL){
                    return ERROR;
                }
                String *note = String_Init(m, STRING_EXTEND);
                String_AddBytes(m, note, bytes("saved bytes:"), strlen("saved bytes:"));
                String_Add(m, note, String_FromI64(m, total));
                String_AddBytes(m, note, bytes(" time:"), strlen(" time:"));
                String_Add(m, note, String_FromI64(m, Time64_Now()));
                String_AddBytes(m, note, bytes("\n"), 1);
                fwrite(note->bytes, 1, note->length, f);
                fclose(f);
            }
            return SUCCESS;
        }
    }
    return NOOP;
}

status File_Delete(File *file){
    char buff[PATH_BUFFLEN];
    file->type.state &= ~FILE_UPDATED;
    file->data = NULL;
    if(unlink((char *)file->abs->bytes) == 0){
        if((file->type.state & FILE_TRACKED) != 0){
            int suffixL = strlen(".notes");
            memcpy(buff, file->abs->bytes, file->abs->length);
            memcpy(buff+file->abs->length, ".notes", suffixL);
            buff[file->abs->length+suffixL] = '\0';
            if(unlink(buff) != 0){
                return ERROR;
            }
        }
        return SUCCESS;
    }
    return ERROR;
}

File *File_Make(MemCtx *m, String *path, Access *access, IoCtx *ctx){
    File *file = MemCtx_Alloc(m, sizeof(File));
    file->type.of = TYPE_FILE;
    file->access = access;
    file->path = path;
    if(ctx != NULL){
        file->abs = IoCtx_GetPath(m, ctx, file->path);
        Span_Add(ctx->files, (Abstract *)file);
    }
    
    return file;
}
