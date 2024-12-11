#include <external.h>
#include <caneka.h>

status SubProcess(MemCtx *m, Span *cmd_p, String *msg_s){
    char **cmd = Span_ToCharArr(m, cmd_p);
    char *msg = String_ToChars(m, msg_s);

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
        execvp(cmd[0], cmd);
        Fatal("Execv failed", 0);
        return ERROR;
    }

    do {
        r = 0;
        p = waitpid(child, &r, 0);
        if(p == (pid_t)-1 && errno != EINTR){
            Fatal("subProcess failed for SubProcess", 0); 
            break;
        }
    } while(p != child);

    if(p != child || !WIFEXITED(r)){
        Fatal("subProcess failed for SubProcess process did not exit propery", 0); 
        return FALSE;
    }

    int code = WEXITSTATUS(r);    
    if(code != 0){
        Fatal("subProcess failed for obj SubProcess", 0); 
        return ERROR;
    }

    return SUCCESS;
}

