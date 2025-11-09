#include <external.h>
#include <caneka.h>

status File_Unlink(MemCh *m, Str *path){
    return unlink(Str_Cstr(m, path)) == 0 ? SUCCESS : ERROR;
}

status File_Open(Buff *bf, Str *fpath, word ioFlags){
    void *args[3];
    char *cstr = Str_Cstr(bf->m, fpath);
    if((ioFlags & (O_TRUNC|O_RDONLY)) || (ioFlags & O_CREAT) == 0){
        i32 ri = stat(cstr, &bf->st); 
        args[0] = bf;
        args[1] = fpath;
        args[2] = NULL;
        if(ri != -1 && (ioFlags & O_TRUNC) && (bf->type.state & BUFF_CLOBBER) == 0){
            Error(bf->m, FUNCNAME, FILENAME, LINENUMBER,
                "File exists and Buff CLOBBER flag us unset for @ @", args);
            bf->type.state |= ERROR;
            return bf->type.state;
        }else if(ri == -1 && (ioFlags & O_RDONLY)){
            Error(bf->m, FUNCNAME, FILENAME, LINENUMBER,
                "File not found for @ @", args);
            bf->type.state |= ERROR;
            return bf->type.state;
        }
    }
    i32 fd = 0;
    if(ioFlags & O_CREAT){
        fd = open(cstr, ioFlags, 0644);
    }else{
        fd = open(cstr, ioFlags);
    }
    if(fd <= 0){
        args[0] = fpath;
        args[1] = Str_CstrRef(bf->m, strerror(errno));
        args[2] = NULL;
        Error(bf->m, FUNCNAME, FILENAME, LINENUMBER,
            "Error opening file @: $", args);
        return ERROR;
    }
    return Buff_SetFd(bf, fd);
}

status File_Close(Buff *bf){
    if(bf->type.state & (BUFF_FD|BUFF_SOCKET)){
        bf->type.state &= ~(BUFF_FD|BUFF_SOCKET);
    }else{
        void *args[] = {
            bf,
            NULL
        };
        Error(bf->m, FUNCNAME, FILENAME, LINENUMBER,
            "Error File_Close Buff does not have a BUFF_SOCKET or BUFF_FD flag @", args);
        return ERROR;
    }

    if(close(bf->fd)){
        bf->type.state |= ERROR;
        return bf->type.state;
    }
    return SUCCESS;
}

boolean File_Exists(Buff *bf, Str *path){
    return stat(Str_Cstr(bf->m, path), &bf->st) == 0 ? TRUE : FALSE;
}

StrVec *File_ToVec(MemCh *m, Str *path){
    Buff *bf = Buff_Make(m, BUFF_SLURP);
    File_Open(bf, path, O_RDONLY);
    Buff_Read(bf);
    if(path->type.state & DEBUG){
        void *args[] = { path, bf, NULL };
        Out("^p.Reading @ to vec: &^0\n", args);
    }
    File_Close(bf);
    return bf->v;
}
