/* remaining header setup */

#define TRUE 1
#define FALSE 0

#define MSG_COLOR 33
#define DEBUG_COLOR 35
#define ERR_COLOR 31
#define DONE_COLOR 32
#define STRMAX 1024
#define ARRMAX 64

static int REBUILD_ALL = FALSE;
static int REBUILD_BINARY = FALSE;

typedef struct cstr {
    char content[STRMAX];
    int length;
} Cstr;

typedef struct str_arr {
    char *arr[ARRMAX];
    int nvalues;
} StrArr;

static Cstr lib_cstr;

/* util functions */
static void Fatal(int code, char *msg, ...){
	va_list args;
    va_start(args, msg);
    if(ERR_COLOR){
        printf("\x1b[%dm", ERR_COLOR);
    }
    printf("Fatal: ");
    vprintf(msg, args);
    if(ERR_COLOR){
        printf("\x1b[0m");
    }
    printf("\n");

    exit(code);
}

static void Debug(char *msg, ...){
	va_list args;
    va_start(args, msg);
    if(ERR_COLOR){
        printf("\x1b[%dm", DEBUG_COLOR);
    }
    vprintf(msg, args);
    if(DEBUG_COLOR){
        printf("\x1b[0m");
    }
}

int Cstr_Add(Cstr *s, char *str){
    int l = strlen(str);
    if((s->length+l) > (STRMAX-1)){
        Fatal(1, "String exceeds max length");
    }
    memcpy(s->content+s->length, str, l);
    s->length += l;
    return TRUE;
}

int Cstr_Init(Cstr *cstr, char *str){
    memset(cstr, 0, sizeof(Cstr));
    if(str != NULL){
        Cstr_Add(cstr, str);
    }
    return TRUE;
}

int Arr_Init(StrArr *arr, char *str){
    memset(arr, 0, sizeof(StrArr));
    if(str != NULL){
        arr->arr[0] = str;
        arr->nvalues = 1;
    }
    return TRUE;
}

int Arr_Add(StrArr *arr, char *str){
    if(arr->nvalues+1 >= ARRMAX-1){
        Fatal(1, "Array max exceeded");
    }
    arr->arr[arr->nvalues] = str;
    arr->nvalues++;
    return TRUE;
}

int Arr_AddArr(StrArr *arr, char *str[]){
    while(*str != NULL){
        Arr_Add(arr, *str);
        str++;
    }
    return TRUE;
}

/* build functions */
static int Cstr_AddBuildName(Cstr *cstr, char *fname){
    Cstr_Add(cstr, fname);
    if(cstr->content[cstr->length-2] == '.' && cstr->content[cstr->length-1] == 'c'){
        cstr->content[cstr->length-1] = 'o';
        return TRUE;
    }
    return FALSE;
}

static int NeedsBuild(Cstr *source_cstr, Cstr *build_cstr){
    struct stat source_stat;
    struct stat build_stat;
    int r = 0;

    r = stat(source_cstr->content, &source_stat);
    if(r != 0){
        Fatal(1, "Source not found %s", source_cstr->content);
        exit(1);
    }
    r = stat(build_cstr->content, &build_stat);
    time_t build_mtime = 0;
    if(r == 0){
        build_mtime = build_stat.st_mtime;
    }

    if(source_stat.st_mtime > build_mtime){
        return TRUE;
    }else{
        return FALSE;
    }
}

static int subProcess(StrArr *cmd_arr, char *msg){
    pid_t child, p;
    int r;

    if(VERBOSE > 1){
        char **cmd = cmd_arr->arr;
        char **arg = &(cmd[1]);
        Debug("%s ", cmd[0]);
        while (*arg != NULL){
            Debug("%s ", *arg);
            arg++;
        }
        printf("\n");
    }

    child = fork();
    if(child == (pid_t)-1){
        Fatal(1, "Fork for %s", msg); 
    }else if(!child){
        char **cmd = cmd_arr->arr;
        execvp(cmd[0], cmd);
        Fatal(1, "Execv failed\n");
        return FALSE;
    }

    do {
        r = 0;
        p = waitpid(child, &r, 0);
        if(p == (pid_t)-1 && errno != EINTR){
            Fatal(1, "subProcess failed for  %s", msg); 
            break;
        }
    } while(p != child);

    if(p != child || !WIFEXITED(r)){
        Fatal(1, "subProcess failed for %s process did not exit propery", msg); 
        return FALSE;
    }

    int code = WEXITSTATUS(r);    
    if(code != 0){
        Fatal(1, "subProcess failed for obj %s return code %d", msg, code); 
        return FALSE;
    }

    return TRUE;
}

static int FolderMake(char *dirname){
    Cstr dir_cstr;
    Cstr_Init(&dir_cstr, "build/");
    Cstr_Add(&dir_cstr, dirname);

    DIR* dir = opendir(dir_cstr.content);
    if(dir){
        closedir(dir);
        printf("Folder exists\n");
    }else if(ENOENT == errno){
        printf("\x1b[%dmMaking Directory %s\x1b[0m\n", MSG_COLOR, dir_cstr.content);

        StrArr mkdir;
        Arr_Init(&mkdir, MKDIR);
        Arr_Add(&mkdir, "-p");
        Arr_Add(&mkdir, dir_cstr.content);
        if(!subProcess(&mkdir, "Folder Make")){
            return FALSE;
        }
    }
    return TRUE;
}


static int BuildObj(char *objName, StrArr *cmd_arr){
    return subProcess(cmd_arr, objName);
}

static int BuildSource(char *binary, char *fname, char *subdir){
    StrArr arr;

    Cstr build_cstr;
    Cstr_Init(&build_cstr, "build/");
    Cstr_Add(&build_cstr, subdir);
    Cstr_Add(&build_cstr, "/");
    Cstr_AddBuildName(&build_cstr, fname);

    Cstr source_cstr;
    Cstr_Init(&source_cstr, "src/");
    Cstr_Add(&source_cstr, subdir);
    Cstr_Add(&source_cstr, "/");
    Cstr_Add(&source_cstr, fname);

    if(NeedsBuild(&source_cstr, &build_cstr)){
        REBUILD_BINARY = TRUE;
        Arr_Init(&arr, CC);
        Arr_AddArr(&arr, CFLAGS);
        Arr_AddArr(&arr, INC);
        Arr_Add(&arr, "-c");
        Arr_Add(&arr, "-o");
        Arr_Add(&arr, build_cstr.content);
        Arr_Add(&arr, source_cstr.content);

        if(VERBOSE){
            printf("\x1b[%dmBuilding file %s -> %s\x1b[0m\n", MSG_COLOR, source_cstr.content, build_cstr.content);
        }

        if(BuildObj(source_cstr.content, &arr)){
            Arr_Init(&arr, AR);
            Arr_Add(&arr, "-rc");
            Arr_Add(&arr, lib_cstr.content);
            Arr_Add(&arr, build_cstr.content);

            return subProcess(&arr, build_cstr.content);
        }
    }
    return FALSE;
}

static int BuildBinary(char *binaryName, char *sourceName){
    printf("\x1b[%dmBuilding binary %s\x1b[0m\n", MSG_COLOR, binaryName);
    StrArr arr;

    Cstr binary_cstr;
    Cstr_Init(&binary_cstr, "build/");
    Cstr_Add(&binary_cstr, binaryName);

    Cstr source_cstr;
    Cstr_Init(&source_cstr, "src/");
    Cstr_Add(&source_cstr, sourceName);

    if(!REBUILD_BINARY && !NeedsBuild(&source_cstr, &binary_cstr)){
        return FALSE;
    }

    Arr_Init(&arr, CC);
    Arr_AddArr(&arr, CFLAGS);
    Arr_AddArr(&arr, INC);
    Arr_Add(&arr, "-o");
    Arr_Add(&arr, binary_cstr.content);
    Arr_Add(&arr, source_cstr.content);
    Arr_Add(&arr, lib_cstr.content);

    return subProcess(&arr, binary_cstr.content);
}

int BuildLib(){
    Cstr_Init(&lib_cstr, "build/lib");
    Cstr_Add(&lib_cstr, BINARY);
    Cstr_Add(&lib_cstr, ".a");

    BuildSubdir **set = ALL;
    while(*set != NULL){
        BuildSubdir *dir = *set;
        FolderMake(dir->name);
        char **fname = dir->sources;
        while(*fname != NULL){
            BuildSource(BINARY, *fname, dir->name);
            fname++;
        }
        set++;
    }
    return TRUE;
}

int Build(){
    if(BuildLib()){
        BuildBinary("testreq", "tests/testreq.c");
        return BuildBinary(BINARY, MAIN);
    }
    return FALSE;
}

int Clean(){
    BuildSubdir **set = ALL;
    char *name = "cleaning source objects"; 
    Cstr path;
    StrArr rm;
    while(*set != NULL){
        Cstr_Init(&path, "build/");
        Cstr_Add(&path, (*set)->name);

        Arr_Init(&rm, RM);
        Arr_Add(&rm, "-rfv");
        Arr_Add(&rm, path.content);
        set++;
        if(!subProcess(&rm, name)){
            return FALSE;
        }
    }

    Arr_Init(&rm, RM);
    Arr_Add(&rm, "-rfv");
    Cstr_Init(&path, "build/");
    Cstr_Add(&path, BINARY);
    Arr_Add(&rm, path.content);
    if(!subProcess(&rm, name)){
        return FALSE;
    }

    Arr_Init(&rm, RM);
    Arr_Add(&rm, "-rfv");
    Cstr_Init(&path, "build/");
    Cstr_Add(&path, "lib");
    Cstr_Add(&path, BINARY);
    Cstr_Add(&path, ".a");
    Arr_Add(&rm, path.content);
    if(!subProcess(&rm, name)){
        return FALSE;
    }

    return TRUE;
}

void **cmdSet[] = {
    (void *)"clean", (void *)Clean,
    (void *)"static", (void *)BuildLib,
    (void *)"build", (void *)Build,
    NULL, NULL
};

/* main */
int main(int argc, char *argv[]){
    if(argc > 1){
        int i = 0;
        for(int ai = 1; ai < argc; ai++){
            char *arg = argv[ai];
            for(; cmdSet[i] != NULL; i+=2){
                char *cmd = (char *)cmdSet[i];
                printf("cmd %s vs arg %s\n", cmd, arg);
                Anon func = (Anon)cmdSet[i+1];
                if(!strcmp(arg, cmd)){
                    printf("Running %s\n", cmd);
                    if(!func()){
                        break;
                    }
                }
            }
        }
    }else{
        Build();
    }
    printf("\x1b[%dmAll Done\x1b[0m\n", DONE_COLOR);
}
