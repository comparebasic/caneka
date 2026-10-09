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

    i32 max = 400;

    for(i32 i = 0; i < max; i++){
        i32 *ip = MemCh_Alloc(NULL, sizeof(i32));
        *ip = i;
        Span_Set(p, i, ip);
    }

    printf("All %d inserted\n", max);
    fflush(stdout);

    for(i32 i = 0; i < max; i++){
        i32 *ip = Span_Get(p, i);
        if(ip == NULL){
            printf("Mismatch i:%d is null\n", i);
            exit(1);
        }else if(*ip != i){
            printf("Mismatch i:%d vs ip:%d\n", i, *ip);
            exit(1);
        }
    }

    printf("All fetched %d!\n", max);
    fflush(stdout);

    p = Span_Make(m, ZERO);

    for(i32 i = 0; i < max; i++){
        i32 *ip = MemCh_Alloc(NULL, sizeof(i32));
        *ip = i;
        Span_Add(p, ip);
    }

    printf("All %d added\n", max);
    fflush(stdout);

    for(i32 i = 0; i < max; i++){
        i32 *ip = Span_Get(p, i);
        if(ip == NULL){
            printf("Mismatch i:%d is null\n", i);
            exit(1);
        }else if(*ip != i){
            printf("Mismatch i:%d vs ip:%d\n", i, *ip);
            exit(1);
        }
    }

    printf("All fetched %d!\n", max);
    fflush(stdout);


    return 1;
}
