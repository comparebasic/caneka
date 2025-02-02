#include <external.h>
#include <caneka.h>

status SubCall(MemCtx *m, Span *cmd_p, ProcDets *pd){
    char **cmd = Span_ToCharArr(m, cmd_p);
    char *msg = "";

    int p0[2];
    int p1[2];
    int p2[2];

    if(pd != NULL){
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
            printf("Unable to open SubCall pipes"); 
            return ERROR;
        }
    }

    pid_t child, p;
    int r;

    if(DEBUG_SUBPROCESS){
        char **arg = &(cmd[1]);
        printf("\x1b[%dmSubProcess \x1b[1;%dm%s\x1b[0m ", 
            DEBUG_SUBPROCESS, DEBUG_SUBPROCESS, cmd[0]);
        while (*arg != NULL){
            printf("\x1b[%dm%s \x1b[0m", DEBUG_SUBPROCESS, *arg);
            arg++;
        }
        printf("\n");
    }


    child = fork();
    if(child == (pid_t)-1){
        printf("Error: Fork for subprocess"); 
        return ERROR;
    }else if(!child){
        if(pd != NULL){
            close(0);
            close(1);
            close(2);
            dup2(p0[0], 0);
            close(p0[1]);
            dup2(p1[1], 1);
            close(p1[0]);
            dup2(p2[1], 2);
            close(p2[0]);
        }
        execvp(cmd[0], cmd);
        printf("Execv failed");
        return ERROR;
    }

    if(pd != NULL){
        close(p0[0]);
        close(p1[1]);
        close(p2[1]);
        pd->inFd = p0[1];
        pd->outFd = p1[0];
        pd->errFd = p2[0];
        pd->pid = child;
    }

    return SUCCESS;
}

status SubStatus(ProcDets *pd){
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
            Fatal("subProcess wait failed for SubProcess", 0); 
        }
        return NOOP;
    }

    if(!WIFEXITED(r)){
        Fatal("subProcess failed for SubProcess process did not exit propery", 0); 
        return ERROR;
    }

    pd->code = WEXITSTATUS(r);    
    if(pd->code != -1){
        return SUCCESS;
    }else{
        return NOOP;
    }
}

status SubProcess(MemCtx *m, Span *cmd_p, ProcDets *pd){
    pid_t pid = SubCall(m, cmd_p, pd);
    if(pid != -1){
        return SubStatus(pd);
    }
    return ERROR;
}

