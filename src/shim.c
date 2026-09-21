#include <external.h>
#include <base_module.h>

word NORMAL_FLAGS = 0b0000000011111111;
word UPPER_FLAGS = 0b1111111100000000;

void Fatal(MemCh *m, char *func, char *file, int line, char *fmt, void *args[]){
    printf("Fatal %s %s %d %s", func, file, line, fmt);
    exit(1);
}

void Error(MemCh *m, char *func, char *file, int line, char *fmt, void *args[]){
    Fatal(m, func, file, line, fmt, args);
}


i32 main(i32 argv, char *args[]){
    return 1;
}
