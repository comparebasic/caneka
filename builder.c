#include "artifact/src/include/external.h"

#if defined(__clang__) 
    #define COMPILER "clang"
#elif defined(__GNUC__) || defined(__GNUG__)
    #define COMPILER "gcc"
#elif define(__MSC_VER)    
    #define COMPILER "msvc"
#endif

#define PROC_MAX 1

#define READY 0
#define SUCCESS 1
#define ERROR 2
#define NOOP 4
#define DONE 8

typedef unsigned char state;

typedef struct proc {
    int pid;
    int code;
    char **cmd;
    int idx;
} Proc;

static Proc *list[16] = {
    NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL,
};

static const char *mkDirCmd[] = {
    "mkdir", "-p", "./build/libcnkbase/",
    NULL
};

static const char *coreCmd[] = {
    COMPILER, "-g", "-Wno-gnu-folding-constant", "-I", "artifact/src/include", "-c", "-o", "./build/libcnkbase/libcnkbase.a",
    "./artifact/src/base/inc.c", 
    NULL
};

static const char *testsCoreCmd[] = {
    COMPILER, "-g", "-I", "./artifact/src/include", "-I", "./artifact/src/programs/basetests/include", "-o", "./build/basetests", 
    "./artifact/src/programs/basetests/tests/inc.c", "./artifact/src/programs/basetests/main.c", "./build/libcnkbase/libcnkbase.a",
    NULL
};

static char *runTestsCoreCmd[] = {
    "./build/basetests",
    NULL
};

static void showMsg(char *name, char **sources){
    printf("%s", name);
    char **p = sources;
    printf("...\n");
    fflush(stdout);
}

static void successMsg(char *name, char **sources){
    printf("\r\x1b[1A\x1b[0K");
    printf("%s", name);
    char **p = sources;
    printf("... \x1b[32mdone\n");
    printf("\x1b[0m");
    fflush(stdout);
}

static void failureMsg(char *name, char **sources){
    printf("\r\x1b[0K\x1b[31m");
    printf("%s:", name);
    char **p = sources;
    while(*p != NULL){
        printf(" %s", *p++);
    }
    printf("... error\n");
    printf("\x1b[0m");
    fflush(stdout);
}


static state process(Proc *pd){
    pid_t child, p;

    int r;
    char **cmd = pd->cmd;

    child = fork();
    if(child == (pid_t)-1){
        printf("Error: Fork for subprocess"); 
        return ERROR;
    }else if(!child){
        execvp(cmd[0], cmd);
        printf("Execv failed for '%s\n", cmd[0]);
        return ERROR;
    }

    pd->pid = child;
    return SUCCESS;
}

static state procStatus(Proc *pd){
    int r;
    pid_t p;
    do {
        r = 0;
        p = waitpid(pd->pid, &r, WNOHANG);
        if(p == (pid_t)-1 && errno != EINTR){
            break;
        }
    } while(p != pd->pid);

    if(p != pd->pid){
        return NOOP;
    }

    if(!WIFEXITED(r)){
        printf("subProcess failed for SubProcess process did not exit propery of %d", pd->pid); 
        return ERROR;
    }

    pd->code = WEXITSTATUS(r);    
    if(pd->code == 0){
        return SUCCESS;
    }else{
        return ERROR;
    }

    return NOOP;
}

static int delay(){
    struct timespec ts;
    struct timespec remains;
    ts.tv_sec = 0;
    ts.tv_nsec = 10000000L;
    return nanosleep(&ts, &remains);
}

static state enqueue(Proc *pd){
    Proc *available;
    int idx = 0;
    while(idx < PROC_MAX){
        available = list[idx];
        if(available == NULL){
            pd->idx = idx;
            list[idx] = pd;
            return process(pd);
        }
        idx++;
    }
    return NOOP;
}

static state run(char *name, char **sources){
    showMsg(name, sources);
    Proc pd = {0, -1, sources, -1};
    while((enqueue(&pd) & (SUCCESS|ERROR)) == 0){ delay(); }
    while((procStatus(&pd) & (SUCCESS|ERROR)) == 0){ delay(); }
    list[pd.idx] = NULL;
    if(pd.code == 0){
        successMsg(name, sources);
    }else{
        failureMsg(name, sources);
        exit(1);
    }
    return SUCCESS;
}

int main(int argc, char *argv[]){
    run("Make build dir", (char **)mkDirCmd);
    run("Building core", (char **)coreCmd);
    run("Building core tests", (char **)testsCoreCmd);
    run("Run tests", (char **)runTestsCoreCmd);
    exit(0);
}
