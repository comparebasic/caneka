#include <external.h>
#include <caneka.h>

StrVec *File_GetVec(File *f){
    return f->sm->dest.curs->v;
}

Cursor *File_GetCurs(File *f){
    return f->sm->dest.curs;
}

status File_Write(File *f, i64 max){
    f->type.state &= ~SUCCESS;

    if(f->sm == NULL){
        Error(ErrStream->m, (Abstract *)f, FUNCNAME, FILENAME, LINENUMBER,
            "Stream not defined for file", NULL);
        return ERROR;
    }

    if((f->sm->type.state & STREAM_TO_FD) == 0){
        Error(ErrStream->m, (Abstract *)f, FUNCNAME, FILENAME, LINENUMBER,
            "Stream not defined for file descriptor persistance, wrong flags?", NULL);
        return ERROR;
    }

    f->sm->func = Stream_ToFd;
    Cursor *curs = f->sm->dest.curs;
    if(Stream_VecTo(f->sm, curs->v) == curs->v->total){
        f->type.state |= SUCCESS;
    }
    return f->type.state;
}

status File_Read(File *f, i64 max){
    f->type.state &= ~SUCCESS;
    if(f->sm == NULL){
        Error(ErrStream->m, (Abstract *)f, FUNCNAME, FILENAME, LINENUMBER,
            "Stream not defined for file", NULL);
        return ERROR;
    }

    if(f->sm->func == NULL){
        Error(ErrStream->m, (Abstract *)f, FUNCNAME, FILENAME, LINENUMBER, 
            "Error no stream func defined for file", NULL);
        return ERROR;
    }

    if(f->sm->fd == -1){
        Abstract *args[] = {
            (Abstract *)f->path,
            NULL
        };
        Error(ErrStream->m, (Abstract *)f, FUNCNAME, FILENAME, LINENUMBER, 
            "Error no file descriptor for stream path: @", args);
        return ERROR;
    }

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

status File_Open(File *f){
    i32 ioFlags = 0;
    i32 mode = 0;
    if((f->type.state & (STREAM_STRVEC|STREAM_TO_FD)) == (STREAM_STRVEC|STREAM_TO_FD)){
        ioFlags = O_RDWR;
    }else if (f->type.state & STREAM_STRVEC){
        ioFlags = O_RDONLY;
    }else if(f->type.state & STREAM_TO_FD){
        ioFlags = O_WRONLY;
    }else{
        Abstract *args[] = {
            (Abstract *)StreamTask_Make(ErrStream->m, NULL, (Abstract *)f, ToS_FlagLabels),
        };
        Error(ErrStream->m, (Abstract *)f, FUNCNAME, FILENAME, LINENUMBER, 
            "STRVEC or TO_FD flags required for read or write or both access to a file descriptor, have $", args);
        return ERROR;
    }

    if(f->type.state & STREAM_CREATE){
        ioFlags |= O_CREAT;
        mode = 0644;
    }

    i32 fd = open(Str_Cstr(f->m, f->path), ioFlags, mode);
    if(fd != -1){
        if(f->type.state & STREAM_TO_FD){
            StrVec *v = NULL;
            if(f->type.state & STREAM_STRVEC){
                v = StrVec_Make(f->m);
            }
            f->sm = Stream_MakeToFd(f->m, fd, v, f->type.state);
        }else if(f->type.state & STREAM_STRVEC){
            f->sm = Stream_MakeStrVec(f->m);
            f->sm->fd = fd;
        }
        f->type.state |= SUCCESS;
    }else{
        f->type.state |= ERROR;
        Abstract *args[] = {
            (Abstract *)f,
            NULL
        };
        Error(ErrStream->m, (Abstract *)f, FUNCNAME, FILENAME, LINENUMBER, 
            "Unable to open file $", args);
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

File *File_Make(MemCh *m, Str *path, Access *access, word flags){
    File *file = MemCh_Alloc(m, sizeof(File));
    file->type.of = TYPE_FILE;
    file->type.state |= flags;
    file->path = path;
    file->access = access;
    file->m = m;
    return file;
}
