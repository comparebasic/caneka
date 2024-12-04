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

#define COLOR_RED 31
#define COLOR_GREEN 32
#define COLOR_YELLOW 33
#define COLOR_BLUE 34
#define COLOR_PURPLE 35
#define COLOR_CYAN 36
#define COLOR_DARK 37

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
