#define TRUE 1
#define FALSE 0

#define IS_FILE 8
#define IS_DIR 4

#define STRMAX 1024
#define ARRMAX 64

#define MSG_COLOR 33
#define DEBUG_COLOR 35
#define ERR_COLOR 31
#define DONE_COLOR 32

typedef struct cstr {
    char content[STRMAX];
    int length;
} Cstr;

typedef struct str_arr {
    char *arr[ARRMAX];
    int nvalues;
} StrArr;

typedef int (*DoCstr)(Cstr *cstr);
typedef int (*Do2Cstr)(Cstr *a, Cstr *b);


enum match_type {
    MATCH_ALL = 0,
    MATCH_END = 1,
    MATCH_START = 2,
};

static int Cstr_Match(char *cstr, char *match, int type){
    int l = strlen(cstr);
    int ml = strlen(match);
    if(ml > l){
        return FALSE;
    }
    char *p = cstr;
    char *m = match;
    if(type == MATCH_END){
        p = cstr+(l - ml);
    }
    for(int i = 0; i < ml; i++){
        if(*p++ != *m++){
            return FALSE;
        }
    }
    return TRUE;
}

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

static int FolderClimb(Cstr *dir_cstr, DoCstr dir, Do2Cstr files){
    struct dirent *ent;
    DIR *d = opendir(dir_cstr->content);
    if(d != NULL){
        while((ent = readdir(d)) != NULL){
            if(ent->d_name[0] == '.' && ent->d_name[0] == '.' || ent->d_name[0] == '0'){
                continue;
            }
            if(ent->d_type == IS_DIR){
                Cstr cstr;
                Cstr_Init(&cstr, dir_cstr->content);
                Cstr_Add(&cstr, "/");
                Cstr_Add(&cstr, ent->d_name);
                dir(&cstr);
                FolderClimb(&cstr, dir, files);
            }else{
                Cstr file_cstr;
                Cstr_Init(&file_cstr, ent->d_name);
                files(dir_cstr, &file_cstr);
            }
        }
        closedir(d);
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

