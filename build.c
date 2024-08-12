#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>

#define TRUE 1
#define FALSE 0

typedef struct build_subdir {
    char *name;
    char *sources[];
} BuildSubdir;

#include "build_config.h"

#define MSG_COLOR 33
#define ERR_COLOR 31
#define DONE_COLOR 32

struct build_cmd {
   char *cc; 
   char *cflags;
   char *inc;
   char *source;
   char *out;
} cmd;

static int FolderMake(char *dirname){
    DIR* dir = opendir(dirname);
    if(dir){
        closedir(dir);
    }else if(ENOENT == errno){
        printf("\x1b[%dmMaking Directory %s\x1b[0m\n", MSG_COLOR, dirname);
    }
    return TRUE;
}
static int NeedsBuild(char *fname){
    return TRUE;
}

static int BuildSource(char *fname){
    printf("\x1b[%dmBuilding file %s\x1b[0m\n", MSG_COLOR, fname);
}

static int BuildBinary(char *binaryName, BuildSubdir *obj[]){
    printf("\x1b[%dmBuilding binary %s\x1b[0m\n", MSG_COLOR, binaryName);
}

int main(){
    BuildSubdir **set = ALL;
    while(*set != NULL){
        BuildSubdir *dir = *set;
        FolderMake(dir->name);
        char **fname = dir->sources;
        while(*fname != NULL){
            if(NeedsBuild(*fname)){
                BuildSource(*fname);
            }
            fname++;
        }
        set++;
    }
    BuildBinary(BINARY, ALL);
    printf("\x1b[%dmDone building\x1b[0m\n", DONE_COLOR);
}
