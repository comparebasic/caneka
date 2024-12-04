#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdarg.h>
#include <unistd.h>

#define RM "rm"
#define MKDIR "mkdir"

#define CRYPTO 1

typedef int (*Anon)();
int Build();
int Clean();
int BuildLib();

typedef struct build_subdir {
    char *name;
    char *sources[];
} BuildSubdir;
