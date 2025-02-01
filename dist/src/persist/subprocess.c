#include <external.h>
#include <caneka.h>

int SubCall(MemCtx *m, Span *cmd_p, String *msg_s, Abstract *source){
    char **cmd = Span_ToCharArr(m, cmd_p);
    char *msg = "";
    if(msg_s != NULL){
        msg = String_ToChars(m, msg_s);
    }

    int p0[2];
    int p1[2];
    int p2[2];

    boolean isServeProc = source->type.of == TYPE_SERVECTX;
    if(isServeProc){
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
            Fatal("Unable to open SubCall pipes", 0); 
            return 1;
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
        Fatal("Fork for subprocess", 0); 
    }else if(!child){
        if(isServeProc){
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
        Fatal("Execv failed", 0);
        return 1;
    }

    if(source->type.of == TYPE_SERVECTX){
        close(p0[0]);
        close(p1[1]);
        close(p2[1]);
        Serve *sctx = (Serve *)source;
        ProcIoSet *set = ProcIoSet_Make(m);
        set->cmds = cmd_p;
        Serve_StartGroup(sctx, (Abstract *)set);

        Req *req = NULL;
        req = Serve_AddFd(sctx, p0[1], PROCIO_INREQ);
        ProcIoSet_Add((ProcIoSet *)sctx->group, req);

        req = Serve_AddFd(sctx, p1[0], PROCIO_OUTREQ);
        ProcIoSet_Add((ProcIoSet *)sctx->group, req);

        req = Serve_AddFd(sctx, p2[0], PROCIO_ERRREQ);
        ProcIoSet_Add((ProcIoSet *)sctx->group, req);

        Serve_StartGroup(sctx, NULL);
    }
    return child;
}

int SubStatus(int pid, boolean wait){
    int r;
    pid_t p;
    do {
        r = 0;
        p = waitpid(pid, &r, wait ? WNOHANG : 0);
        if(p == (pid_t)-1 && errno != EINTR){
            Fatal("subProcess failed for SubProcess", 0); 
            break;
        }
    } while(p != pid && !wait);

    if(p != pid){
        return -1;
    }

    if(p != pid || !WIFEXITED(r)){
        Fatal("subProcess failed for SubProcess process did not exit propery", 0); 
        return FALSE;
    }

    return  WEXITSTATUS(r);    
}

status SubProcess(MemCtx *m, Span *cmd_p, String *msg_s, Abstract *source){
    pid_t pid = SubCall(m, cmd_p, msg_s, source) ;
    if(pid != -1){
        return SubStatus(pid, TRUE) == 0 ? SUCCESS : ERROR;
    }
    return ERROR;
}

