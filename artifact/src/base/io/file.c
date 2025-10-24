#include <external.h>
#include <caneka.h>

status File_Open(Buff *bf, Str *fpath, word ioFlags){
    char *cstr = Str_Cstr(bf->m, fpath);
    i32 fd = 0;
    if(ioFlags & O_CREAT){
        fd = open(cstr, ioFlags, 0644);
    }else{
        fd = open(cstr, ioFlags);
    }
    if(fd <= 0){
        Abstract *args[] = {
            (Abstract *)fpath,
            (Abstract *)Str_CstrRef(bf->m, strerror(errno)),
            NULL
        };
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
        Abstract *args[] = {
            (Abstract *)bf,
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
