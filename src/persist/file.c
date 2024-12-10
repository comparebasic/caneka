#include <external.h>
#include <caneka.h>

File *File_Clone(MemCtx *m, File *o){
    File *file = MemCtx_Realloc(m, sizeof(File), o, sizeof(File));
    file->path = String_Clone(m, file->path);
    file->abs = String_Clone(m, file->abs);
    file->data = String_Clone(m, file->data);
    return file;
}

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
            if(DEBUG_FILE){
                printf("\x1b[%dmSaved file: %s\n", DEBUG_FILE, file->path->bytes);
            }
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

status File_Read(MemCtx *m, File *file, Access *access, int pos, int length){
    int remaining = length;
    FILE *f = fopen((char *)file->abs->bytes, "r");
    if(f == NULL){
        file->type.state |= NOOP;
        return file->type.state;
    }else{
        file->type.state &= ~NOOP;
    }

    char *buff[FILE_READ_LENGTH+1];
    memset(buff, 0, FILE_READ_LENGTH+1);

    if(file->data == NULL){
        file->data = String_Init(m, STRING_EXTEND);
    }

    while(remaining > 0){
        int amount = min(remaining, FILE_READ_LENGTH);
        size_t read = fread(buff, 1, amount, f);
        if(read != amount){
            return ERROR;
        }
        remaining -= read;
        String_AddBytes(m, file->data, bytes(buff), read);
    }

    fclose(f);
    return SUCCESS;
}

status File_Load(MemCtx *m, File *file, Access *access){
    FILE *f = fopen((char *)file->abs->bytes, "r");
    if(f == NULL){
        file->type.state |= NOOP;
        return file->type.state;
    }else{
        file->type.state &= ~NOOP;
    }

    file->data = String_Init(m, STRING_EXTEND);
    char buff[FILE_READ_LENGTH+1];
    size_t l = fread(buff, 1, FILE_READ_LENGTH, f);
    while(l > 0){
        String_AddBytes(m, file->data, bytes(buff), l);
        if(l < FILE_READ_LENGTH){
            break;
        }
        l = fread(buff, 1, FILE_READ_LENGTH, f);
    }

    if(feof(f)){
        file->type.state |= FILE_LOADED;
        return SUCCESS;
    }else if(ferror(f)){
        return ERROR;
    }

    return ERROR;
}

status File_Delete(File *file){
    char buff[PATH_BUFFLEN];
    file->type.state &= ~FILE_UPDATED;
    file->data = NULL;
    if(file->abs == NULL){
        printf("no abs path set %s\n", file->path->bytes);
        return ERROR;
    }
    if(DEBUG_FILE){
        printf("\x1b[%dmDeleting File:%s\n", DEBUG_FILE, file->abs->bytes);
    }
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
    struct stat st;
    if(stat((char *)file->abs->bytes, &st) == -1){
        file->type.state |= NOOP;
        return file->type.state;
    }
    file->type.state |= ERROR;
    return file->type.state;
}

File *File_Init(File *file, String *path, Access *access, IoCtx *ctx){
    memset(file, 0, sizeof(File));
    file->type.of = TYPE_FILE;
    file->access = access;
    file->path = path;
    if(ctx != NULL){
        file->abs = IoCtx_GetPath(ctx->m, ctx, file->path);
        Table_Set(ctx->files, (Abstract *)file->path, (Abstract *)file);
    }
    
    return file;
}

File *File_Make(MemCtx *m, String *path, Access *access, IoCtx *ctx){
    File *file = MemCtx_Alloc(m, sizeof(File));
    return File_Init(file, path, access, ctx);
}
