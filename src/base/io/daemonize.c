#include <external.h>
#include <caneka.h>

status Daemonize_StdToFiles(MemCh *m, StrVec *outPath, StrVec *errPath){
    DebugStack_Push(outPath, outPath->type.of);
    status r = READY;

    Buff *bf = Buff_Make(m, ZERO);
    File_Open(bf, outPath, O_WRONLY|O_CREAT|O_APPEND);
    i32 outFd = 1;
    if(bf->fd <= 0){
        DebugStack_Pop();
        return ERROR;
    }
    outFd = bf->fd;

    Buff *ebf = Buff_Make(m, ZERO);
    File_Open(ebf, errPath, O_WRONLY|O_CREAT|O_APPEND);
    i32 errFd = 2;
    if(ebf->fd <= 0){
        DebugStack_Pop();
        return ERROR;
    }
    errFd = bf->fd;

    Core_Direct(m, outFd, errFd);
    DebugStack_Pop();
    return SUCCESS;
}

status Daemonize(MemCh *m,
        SourceFunc func,
        void *arg,
        void *source,
        StrVec *pidPath,
        i32 *pid,
        char **fmt,
        void **args
    ){

    if(pidPath != NULL){
        StrVec *pidV = File_ToVec(m, StrVec_Str(m, pidPath));
        if(pidV != NULL && pidV->total > 0){
            args[0] = pidV;
            args[1] = pidPath;
            args[2] = NULL;
            Str *s = S(m, "Pid already exists $ stored in $\n");
            *fmt = (char *)s->bytes;
            *pid = (i32)I64_FromStr(StrVec_Str(m, pidV));
            return ERROR;
        }
    }

    i32 child = fork();
    if(child == (pid_t)-1){
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "Error spawning webserver", NULL);
        exit(13);
        return 13;
    }else if(child == 0){
        if(OutStream->fd != 1 && ErrStream->fd != 2){
            close(1);
            close(2);
        }
        setsid();

        func(m, arg, source);

        exit(0);
        return 0;
    }

    if(pidPath != NULL){
        Buff *bf = Buff_Make(m, BUFF_UNBUFFERED);
        File_Open(bf, pidPath, O_WRONLY|O_CREAT|O_TRUNC);
        Buff_Add(bf, Str_FromI64(m, child));
        File_Close(bf);
    }

    *pid = child;
    return SUCCESS;
}
