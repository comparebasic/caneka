#include <external.h>
#include <caneka.h>

File *File_Clone(MemCtx *m, File *o){
    File *file = MemCtx_Realloc(m, sizeof(File), o, sizeof(File));
    file->path = String_Clone(m, file->path);
    file->abs = String_Clone(m, file->abs);
    file->data = String_Clone(m, file->data);
    return file;
}

status File_Copy(MemCtx *m, String *a, String *b, Access *ac){
    Span *cmd = Span_Make(m, TYPE_SPAN);
    Span_Add(cmd, (Abstract *)String_Make(m, bytes("cp")));
    Span_Add(cmd, (Abstract *)a);
    Span_Add(cmd, (Abstract *)b);

    return SubProcess(m, cmd, NULL);
}

status File_Exists(String *path){
    struct stat st;
    int r = 0;

    r = stat((char *)path->bytes, &st);
    if(r != 0){
        return NOOP;
    }

    return SUCCESS;
}

boolean File_CmpUpdated(MemCtx *m, String *a, String *b, Access *ac){
    struct stat source_stat;
    struct stat build_stat;
    int r = 0;

    r = stat(String_ToChars(m, a), &source_stat);
    if(r != 0){
        Fatal("Source not found %s", TYPE_FILE);
        exit(1);
    }
    r = stat(String_ToChars(m, b), &build_stat);
    time_t build_mtime = 0;
    if(r == 0){
        build_mtime = build_stat.st_mtime;
    }

    if(source_stat.st_mtime > build_mtime){
        return TRUE;
    }else{
        return FALSE;
    }
}

status File_Persist(MemCtx *m, File *file){
    char buff[PATH_BUFFLEN];

    if(file->data != NULL){
        if((file->type.state & FILE_UPDATED) != 0){
            char *mode = "w";
            if((file->type.state & FILE_SPOOL) != 0){
                mode = "a";
            }
            FILE *f = fopen((char *)file->abs->bytes, mode);
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

status File_StreamWithOpen(MemCtx *m, FILE *f, File *file, Access *access, OutFunc out, Abstract *source){
    DebugStack_Push("File_StreamWithOpen", TYPE_CSTR); 
    DebugStack_SetRef(file->abs, file->abs->type.of);
    String *s = NULL;
    if(out != NULL){
        s = String_Init(m, STRING_EXTEND);
    }

    file->data = String_Init(m, STRING_EXTEND);
    char buff[FILE_READ_LENGTH+1];
    size_t l = fread(buff, 1, FILE_READ_LENGTH, f);
    while(l > 0){
        if(DEBUG_FILE){
            printf("\x1b[%dmFile_Stread read %ld bytes\x1b[0m\n", DEBUG_FILE, l);
        }
        if(out != NULL){
            String_Reset(s);
            String_AddBytes(m, s, bytes(buff), l);
            out(m, s, source);
        }else{
            String_AddBytes(m, file->data, bytes(buff), l);
        }
        if(l < FILE_READ_LENGTH){
            break;
        }
        l = fread(buff, 1, FILE_READ_LENGTH, f);
    }

    if(feof(f)){
        file->type.state |= FILE_LOADED;
        DebugStack_Pop();
        return SUCCESS;
    }else if(ferror(f)){
        DebugStack_Pop();
        return ERROR;
    }

    DebugStack_Pop();
    return ERROR;
}

status File_Stream(MemCtx *m, File *file, Access *access, OutFunc out, Abstract *source){
    DebugStack_Push("File_Stream", TYPE_CSTR); 
    if(DEBUG_FILE){
        printf("\x1b[%dmFile_Stread streaming: '%s'\x1b[0m\n", DEBUG_FILE, file->abs->bytes);
    }
    if(file->abs == NULL){
        Fatal("No Abs path for File object", TYPE_FILE);
        DebugStack_Pop();
        return ERROR;
    }
    FILE *f = fopen((char *)file->abs->bytes, "r");
    if(f == NULL){
        printf("Warning: Unable to openfile '%s'\n", file->abs->bytes);
        file->type.state |= NOOP;
        DebugStack_Pop();
        return file->type.state;
    }else{
        file->type.state &= ~NOOP;
    }

    status r = File_StreamWithOpen(m, f, file, access, out, source);
    DebugStack_Pop();
    return r;
}

status File_Load(MemCtx *m, File *file, Access *access){
    return File_Stream(m, file, access, NULL, NULL);
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
