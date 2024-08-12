#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdarg.h>

#define TRUE 1
#define FALSE 0

typedef struct build_subdir {
    char *name;
    char *sources[];
} BuildSubdir;

#include "build_config.h"

#ifndef VERBOSE 
    #define VERBOSE 0
#endif

#define MSG_COLOR 33
#define ERR_COLOR 31
#define DONE_COLOR 32
#define STRMAX 1024
#define ARRMAX 64

typedef struct cstr {
    char content[STRMAX];
    int length;
} Cstr;

typedef struct str_arr {
    char *arr[ARRMAX];
    int nvalues;
} StrArr;

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
    arr->arr[arr->nvalues] = str;
    arr->nvalues++;
    return TRUE;
}

int Arr_AddArr(StrArr *arr, char *str[]){
    while(*str != NULL){
        arr->arr[arr->nvalues] = *str;
        arr->nvalues++;
        str++;
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
        mkdir(dir_cstr.content, 0577);
    }
    return TRUE;
}

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

static int BuildObj(char *objName, StrArr *cmd){
    if(VERBOSE){
        printf("\x1b[%dmBuilding binary %s\x1b[0m\n", MSG_COLOR, binaryName);
    }
    pid_t child, p;
    int r;

    child = fork();
    if(child == (pid_t)-1){
        Fatal("Fork building obj %s", objName); 
    }else if(!child){
        char *cmd = cmd->arr;
        execv(*cmd, ++cmd);
    }

    do {
        r = 0;
        p = waitpid(child, &r);
        if(p == (pid_t)-1 && errno != EINTR){
            Fatal("Build command failed for obj %s", objName); 
            break;
        }
    } while(p != child);

    if(!WIFEXITED(r)){
        Fatal("Build command failed for obj %s process did not exit propery", objName); 
    }

    int code = WEXITSTATUS(r);    
    if(code != 0){
        Fatal("Build command failed for obj %s return code %d", objName, code); 
    }

    return TRUE;
}

static int BuildSource(char *fname, char *subdir){
    StrArr arr;

    Cstr build_cstr;
    Cstr_Init(&build_cstr, "build/");
    Cstr_Add(&build_cstr, subdir);
    Cstr_Add(&build_cstr, "/");
    Cstr_AddBuildName(&build_cstr, fname);

    Cstr source_cstr;
    Cstr_Init(&source_cstr, subdir);
    Cstr_Add(&source_cstr, "/");
    Cstr_Add(&source_cstr, fname);

    if(NeedsBuild(&source_cstr, &build_cstr)){
        Arr_Init(&arr, CC);
        Arr_AddArr(&arr, cflags);
        Arr_AddArr(&arr, CFLAGS);
        Arr_AddArr(&arr, INC);
        Arr_Add(&arr, "-o");
        Arr_Add(&arr, build_cstr.content);
        Arr_Add(&arr, source_cstr.content);

        if(VERBOSE){
            printf("\x1b[%dmBuilding file %s -> %s\x1b[0m\n", MSG_COLOR, source_cstr.content, build_cstr.content);
        }
        char *arg = arr.arr[0];
        int i = 0;
        if(VERBOSE > 1){
            while (arg != NULL){
                printf("%s ", arg);
                arg = arr.arr[++i];
            }
            printf("\n");
        }

        return TRUE;
    }
    return FALSE;
}

static int BuildBinary(char *binaryName){
    printf("\x1b[%dmBuilding binary %s\x1b[0m\n", MSG_COLOR, binaryName);
    return TRUE;
}

int main(){
    BuildSubdir **set = ALL;
    while(*set != NULL){
        BuildSubdir *dir = *set;
        FolderMake(dir->name);
        char **fname = dir->sources;
        while(*fname != NULL){
            BuildSource(*fname, dir->name);
            fname++;
        }
        set++;
    }
    BuildBinary(BINARY, ALL);
    printf("\x1b[%dmDone building\x1b[0m\n", DONE_COLOR);
}
