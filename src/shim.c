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
    MemCh *m = NULL;
    Span *p = Span_Make(m, ZERO);

    for(i32 i = 0; i < 400; i++){
        i32 *ip = MemCh_Alloc(NULL, sizeof(i32));
        printf("Adding %d %p\n", i, ip);
        fflush(stdout);
        *ip = i;
        Span_Add(p, ip);
    }

    printf("All inserted\n");
    fflush(stdout);

    for(i32 i = 0; i < 400; i++){
        i32 *ip = Span_Get(p, i);
        printf("\x1b[35mGetting idx %d %p\x1b[0m\n", i, ip);
        fflush(stdout);
        if(ip == NULL){
            printf("Mismatch i:%d is null\n", i);
            exit(1);
        }else if(*ip != i){
            printf("Mismatch i:%d vs ip:%d\n", i, *ip);
            exit(1);
        }else{
            printf("\x1b[32mMatch %d\x1b[0m\n", i);
            fflush(stdout);
        }
    }

    printf("All done!");
    fflush(stdout);

    return 1;
}
