#include <external.h>
#include <caneka.h>

static status _File_Init(MemCh *m, File *file, Str *path, Access *access){
    DebugStack_Push(path, path->type.of);
    file->type.of = TYPE_FILE;
    file->path = path;
    file->access = access;
    file->sm = File_MakeFileStream(m, Stream_Make(m), StrVec_Make(m), -1, STREAM_STRVEC|STREAM_TO_FD); 
    DebugStack_Pop();
    return SUCCESS;
}

static status File_MakeFileStream(MemCh *m, Stream *sm, StrVec *v, i32 fd, word flags){
    sm->type.of = TYPE_STREAM;
    sm->m = m;
    sm->fd = fd;
    sm->type.state |= UPPER_FLAGS & flags;
    sm->dest.curs = Cursor_Make(m, v);
    sm->func = Stream_ToStrVec;
    return SUCCESS;
}

status File_Write(File *f, i64 max){
    f->type.state &= ~SUCCESS;
    if(f->sm->fd == -1){
        File_Open(f, f->type.state);
        if((f->type.state & SUCCESS) == 0){
            f->type.state |= ERROR;
            Abstract *args[] = {
                (Abstract *)f->path,
                NULL
            };
            Error(ErrStream->m, (Abstract *)f, FUNCNAME, FILENAME, LINENUMBER, 
                "Unable to open file @", args);
            return f->type.state;
        }
    }
    f->sm->func = Stream_ToFd;
    if(Stream_VecTo(sm, f->sm->v, f->sm->v->total) == f->sm->v->total){
        f->type.state |= SUCCESS;
    }
    return f->type.state;
}

status File_Read(File *f, i64 max){
    f->type.state &= ~SUCCESS;
    if(f->sm->fd == -1){
        File_Open(f, f->type.state);
        if((f->type.state & SUCCESS) == 0){
            f->type.state |= ERROR;
            Abstract *args[] = {
                (Abstract *)f->path,
                NULL
            };
            Error(ErrStream->m, (Abstract *)f, FUNCNAME, FILENAME, LINENUMBER, 
                "Unable to open file @", args);
            return f->type.state;
        }
    }

    f->sm->func = Stream_ToStrVec;

    byte b[FILE_READ_LENGTH];
    while(max > 0){
        ssize_t l = read(f->sm->fd, b, min(FILE_READ_LENGTH, max));
        if(l <= 0){
            if(l < 0){
                Abstract *args[] = {
                    (Abstract *)f->path,
                    NULL
                };
                Error(ErrStream->m, (Abstract *)f, FUNCNAME, FILENAME, LINENUMBER, 
                    "Error reading file @", args);
                    f->type.state |= ERROR;
            }
            break;
        }
        max -= l;
        Stream_Bytes(f->sm, b, l);
    }

    if(max == 0){
        f->type.state |= SUCCESS;
    }else{
        f->type.state |= ERROR;
    }
    return f->type.state;
}

status File_Open(File *f, word flags){
    i32 ioFlags = 0;
    if((flags & (STREAM_STRVEC|STREAM_TO_FD)) == (STREAM_STRVEC|STREAM_TO_FD)){
        ioFlags = O_RDWR;
    }else if (flags & STREAM_STRVEC){
        ioFlags = O_RONLY;
    }else if(flags & STREAM_TO_FD){
        ioFlags = O_WONLY;
    }else{
        Error(ErrStream->m, (Abstract *), FUNCNAME, FILENAME, LINENUMBER, 
            "STRVEC or TO_FD flags required for read or write or both access to a file descriptor", NULL);
        return ERROR;
    }
    f->sm->fd = open(Str_Cstr(f->sm->m, f->path), ioFlags);
    if(f->sm->fd != -1){
        f->type.state |= SUCCESS;
    }else{
        f->type.state |= ERROR;
    }
    return f->type.state;
}

status File_Close(File *f){
    if(f->sm->fd != -1){
        if(!close(f->sm->fd)){
            f->type.state |= ERROR;
        }else{
            f->type.state |= SUCCESS;
        }
    }
    return f->type.state;
}


/*
status File_Copy(MemCh *m, Str *a, Str *b, Access *ac){
    Span *cmd = Span_Make(m);
    Span_Add(cmd, (Abstract *)String_Make(m, bytes("cp")));
    Span_Add(cmd, (Abstract *)a);
    Span_Add(cmd, (Abstract *)b);

    return SubProcess(m, cmd, NULL);
}

status File_Persist(MemCh *m, File *file){
    char buff[PATH_BUFFLEN];
    if(DEBUG_FILE){
        DPrint((Abstract *)file->abs, DEBUG_FILE, "Persisting: ");
    }
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

FILE *File_GetFILE(MemCh *m, File *file, Access *access){
    return fopen((char *)file->abs->bytes, "r");
}

status File_Read(MemCh *m, File *file, Access *access, int pos, int length){
    int remaining = length;
    FILE *f = File_GetFILE(m, file, access);;
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

status File_StreamWithOpen(MemCh *m, FILE *f, File *file, Access *access, OutFunc out, Abstract *source){
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

status File_Stream(MemCh *m, File *file, Access *access, OutFunc out, Abstract *source){
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

status File_Load(MemCh *m, File *file, Access *access){
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
*/

File *File_Make(MemCh *m, Str *path, Access *access){
    File *file = MemCh_Alloc(m, sizeof(File));
    _File_Init(m, file, path, access);
    return file;
}
