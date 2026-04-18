/* Base.io.SubProcess
 *
 * Functions for forking a new process, whose details are stored
 * on a ProcDets object
 */

#include <external.h>
#include "base_module.h"

status SubCall(MemCh *m, Span *cmd_p, ProcDets *pd){
    Debug_Push(m, pd);

    char **cmd = Span_ToCharArr(m, cmd_p);
    char *msg = "";

    int p0[2];
    int p1[2];
    int p2[2];

    if(pd->type.state & PROCDETS_PIPES){
        if((pipe(p0) != 0 || pipe(p1) != 0 || pipe(p2) != 0)
            ||
            (
                (fcntl(p0[0], F_SETFL, O_NONBLOCK) == -1) ||
                (fcntl(p0[1], F_SETFL, O_NONBLOCK) == -1) ||
                (fcntl(p1[0], F_SETFL, O_NONBLOCK) == -1) ||
                (fcntl(p1[1], F_SETFL, O_NONBLOCK) == -1) ||
                (fcntl(p2[0], F_SETFL, O_NONBLOCK) == -1) ||
                (fcntl(p2[1], F_SETFL, O_NONBLOCK) == -1)
            )

        ){
            Return(m, ERROR);
        }
    }else if(pd->type.state & PROCDETS_IN_PIPE){
        if((pipe(p0) != 0)
            ||
            (
                (fcntl(p0[0], F_SETFL, O_NONBLOCK) == -1) ||
                (fcntl(p0[1], F_SETFL, O_NONBLOCK) == -1)
            )

        ){
            Return(m, ERROR);
        }

    }

    pid_t child, p;
    int r;

    child = vfork();
    if(child == (pid_t)-1){
        Return(m, ERROR);
    }else if(!child){
        if(pd->type.state & PROCDETS_PIPES){
            close(0);
            close(1);
            close(2);
            dup2(p0[0], 0);
            close(p0[1]);
            dup2(p1[1], 1);
            close(p1[0]);
            dup2(p2[1], 2);
            close(p2[0]);
        }else if(pd->type.state & PROCDETS_IN_PIPE){
            close(0);
            dup2(p0[0], 0);
            close(p0[1]);
        }
        execvp(cmd[0], cmd);
        exit(1);

        Return(m, ERROR);
    }

    if(pd->type.state & PROCDETS_PIPES){
        close(p0[0]);
        close(p1[1]);
        close(p2[1]);
        pd->inFd = p0[1];
        pd->outFd = p1[0];
        pd->errFd = p2[0];
    }else if(pd->type.state & PROCDETS_IN_PIPE){
        close(p0[0]);
        pd->inFd = p0[1];
    }
    pd->pid = child;

    Return(m, SUCCESS);
}

status SubStatus(ProcDets *pd){
    MemCh *m = pd->m;
    Debug_Push(m, pd);

    int r;
    pid_t p;

    boolean wait = (pd->type.state & PROCDETS_ASYNC) != 0;
    do {
        r = 0;
        p = waitpid(pd->pid, &r, wait ? WNOHANG : 0);
        if(p == (pid_t)-1 && errno != EINTR){
            break;
        }
    } while(p != pd->pid && !wait);

    if(p != pd->pid){
        if(!wait){
            Error(ErrStream->m, FUNCNAME, FILENAME, LINENUMBER,
                "subProcess wait failed for SubProcess", NULL); 
            return ERROR;
        }
        Return(m, NOOP);
    }

    if(!WIFEXITED(r)){
        Error(ErrStream->m, FUNCNAME, FILENAME, LINENUMBER, 
            "subProcess failed for SubProcess process did not exit propery", NULL);

        Return(m, ERROR);
    }

    pd->code = WEXITSTATUS(r);    
    if(pd->code == 0){
        pd->type.state |= SUCCESS;
    }else{
        pd->type.state |= ERROR;
    }

    Return(m, pd->type.state);
}

status SubProcess(MemCh *m, Span *cmd_p, ProcDets *pd){
    Debug_Push(m, cmd_p);

    status r = SubCall(m, cmd_p, pd);
    if(r & SUCCESS){
        status r = SubStatus(pd);

        Return(m, r);
    }
    
    Return(m, ERROR);
}

status SubProcToBuff(MemCh *m, Span *cmd, Buff *out, Buff *err){
    ProcDets pd = {
        .type = {TYPE_PROCDETS, PROCDETS_PIPES|PROCDETS_ASYNC}, 
        0,
        m,
        -1,-1,-1,-1
    };

    status r = SubCall(m, cmd, &pd);
    if((r & SUCCESS) == 0){
        return ERROR;
    }

    if(out != NULL){
        out->type.state |= BUFF_ASYNC;
        Buff_SetFd(out, pd.outFd);
    }

    if(err != NULL){
        err->type.state |= BUFF_ASYNC;
        Buff_SetFd(err, pd.errFd);
    }

    struct timespec current = {0, 0};
    r = ZERO;
    while((r & (ERROR|SUCCESS)) == 0){
        r |= SubStatus(&pd);
        if(out != NULL) Buff_Read(out);
        if(err != NULL) Buff_Read(err);
        Time_Throttle(&current);
    }
    return r;
}
