#include "artifact/src/include/external.h"
#include "artifact/src/include/config/arch.h"

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

typedef struct cstr {
    char *content;
    size_t length;
    size_t remaining;
} Cstr;

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
    "-DINSECURE",
    NULL
};

static const char *mkDirCnkBuildLibCmd[] = {
    "mkdir", "-p", "./build/libcnkbuild/",
    NULL
};

static char *buildBuilderLibCmd[] = {
    COMPILER, "-g", "-I", "./artifact/src/include", "-I", "./artifact/src/programs/cnkbuild/include", "-c", "-o", "./build/libcnkbuild/libcnkbuild.a", 
    "./artifact/src/programs/cnkbuild/cnkbuild.c",
    "-DINSECURE",
    NULL
};

static void showMsg(char *name, char **sources){
    printf("%s", name);
    char **p = sources;
    printf("...\n");
    fflush(stdout);
}

static void successMsg(char *name, char **sources){
    printf("%s", name);
    char **p = sources;
    printf("... \x1b[32mdone\n");
    printf("\x1b[0m");
    fflush(stdout);
}

static void failureMsg(char *name, char **sources, int code){
    printf("\r\x1b[0K\x1b[31m");
    printf("%s:", name);
    char **p = sources;
    while(*p != NULL){
        printf(" %s", *p++);
    }
    printf("... error code %d\n", code);
    printf("\x1b[0m");
    fflush(stdout);
}

static void Cstr_Init(Cstr *cstr){
    memset(cstr, 0, sizeof(Cstr));
    cstr->content = malloc(STR_DEFAULT);
    if(cstr->content == NULL){
        fprintf(stderr, "Error unable to allocate bytes for Cstr\n");
        exit(13);
    }
    cstr->remaining = STR_DEFAULT;
}

static size_t Cstr_Combine(Cstr *cstr, Cstr *add){
    if(cstr->remaining < cstr->length+1){
        fprintf(stderr, "Error no more room left in string %s to add %s\n", 
            cstr->content, add->content);
        exit(13);
    }
    memcpy(cstr->content+cstr->length, add->content, add->length);
    cstr->remaining -= add->length;
    cstr->length += add->length;
    return add->length;
}

static size_t Cstr_Add(Cstr *cstr, char *add){
    size_t l = strlen(add);
    if(cstr->remaining < l+1){
        fprintf(stderr, "Error no more room left in string %s to add %s\n", 
            cstr->content, add);
        exit(13);
    }
    memcpy(cstr->content+cstr->length, add, l);
    cstr->remaining -= l;
    cstr->length += l;
    return l;
}

static int Cstr_EqChars(Cstr *cstr, char *add){
    size_t l = strlen(add);
    if(cstr->length != l){
        return 0;
    }else{
        return strncmp(cstr->content, add, l) == 0;
    }
}

static int Cstr_EndEqChars(Cstr *cstr, char *add){
    size_t l = strlen(add);
    if(cstr->length < l){
        return 0;
    }else{
        return strncmp(cstr->content+(cstr->length-l), add, l) == 0;
    }
}

static int Cstr_Copy(Cstr *dest, Cstr *cstr){
    Cstr_Init(dest);
    memcpy(dest->content, cstr->content, cstr->length);
    dest->length += cstr->length;
    return dest->length;
}

static size_t Cstr_Basename(Cstr *cstr){
    char *p = cstr->content;
    char *last = p+cstr->length;
    char *lastSlash = p;
    while(p < last){
        if(*p == '/'){
            lastSlash = p;
        }
        p++;
    }
    cstr->content = lastSlash+1;
    cstr->length =  last - cstr->content;
    return cstr->length;
}

static int Cstr_Shrink(Cstr *cstr, size_t length){
    if(cstr->length < length){
        fprintf(stderr, "Error original '%s' is shorter than shrink request %ld\n", 
            cstr->content, length);
        exit(13);
    }else{
        memset(cstr->content+(cstr->length-length), 0, length);
        return 1;
    }
}

static int Cstr_Replace(Cstr *cstr, char a, char b){
    char *p = cstr->content;
    char *last = p+cstr->length;
    int count = 0;
    while(p < last){
        if(*p == a){
            *p = b;
            count++;
        }
        p++;
    }
    return count;
}

static int Cstr_Incr(Cstr *cstr, size_t length){
    if(cstr->length < length){
        fprintf(stderr, "Error original '%s' is shorter than incr request %ld\n", 
            cstr->content, length);
        exit(13);
    }
    cstr->content += length;
    cstr->length -= length;
    return length;
}

static size_t Cstr_From(Cstr *cstr, char *add){
    Cstr_Init(cstr);
    return Cstr_Add(cstr, add);
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
        failureMsg(name, sources, pd.code);
        exit(1);
    }
    return SUCCESS;
}

int main(int argc, char *argv[]){
    if(argc < 2){
        printf("builder build\nbuilder ./build.c\n");
        exit(1);
    }
    if(strncmp(argv[1], "build", strlen("build")) == 0){
        run("Make build dir", (char **)mkDirCmd);
        run("Building core", (char **)coreCmd);
        run("Make CnkBuild dir", (char **)mkDirCnkBuildLibCmd);
        run("Build CnkBuild", (char **)buildBuilderLibCmd);
    }else{
        Cstr target;
        Cstr_From(&target, argv[1]);
        printf("target: %s\n", target.content);
        if(Cstr_EndEqChars(&target, "build.c") == 0){
            fprintf(stderr, "Expected build file name to end in 'build.c', have %s\n",
                target.content);
            exit(13);
        }

        Cstr out;
        Cstr_From(&out, "./build/");
        Cstr base;
        Cstr_Copy(&base, &target);
        Cstr_Incr(&base, strlen("artifact/src/"));
        Cstr_Replace(&base, '/', '_');
        Cstr_Combine(&out, &base);
        Cstr_Shrink(&out, strlen(".c"));

        char *cmd[] = {
            COMPILER, "-g", "-I", "./artifact/src/include", "-I", "./artifact/src/programs/cnkbuild/include", "-o", out.content,
            target.content, "./build/libcnkbuild/libcnkbuild.a", "./build/libcnkbase/libcnkbase.a",
            "-lm",
            "-DINSECURE",
            NULL
        };

        Cstr msg;
        Cstr_Init(&msg);
        Cstr_Add(&msg, "Building ");
        Cstr_Combine(&msg, &target);
        Cstr_Add(&msg, " -> ");
        Cstr_Combine(&msg, &out);
        run(msg.content, (char **)cmd);
    }
    exit(0);
}
