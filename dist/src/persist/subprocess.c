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
        if(source->type.of == TYPE_SERVECTX){
            Serve *sctx = (Serve *)source;
            dup2(0, p0[1]);
            dup2(1, p1[1]);
            dup2(2, p2[1]);

            ProcIoSet *set = ProcIoSet_Make(m);
            set->cmds = cmd_p;
            Serve_StartGroup(sctx, (Abstract *)set);

            Req *req = NULL;

            req = Serve_AddFd(sctx, p0[0], PROCIO_INREQ);
            ProcIoSet_Add(sctx->group, req);

            req = Serve_AddFd(sctx, p1[0], PROCIO_OUTREQ);
            ProcIoSet_Add(sctx->group, req);

            req = Serve_AddFd(sctx, p2[0]);
            ProcIoSet_Add(sctx->group, req, PROCIO_ERRREQ);

            Serve_StartGroup(sctx, NULL);
        }
        
        execvp(cmd[0], cmd);
        Fatal("Execv failed", 0);
        return 1;
    }else{
        return child;
    }

    return 1;
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

