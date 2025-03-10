#include <external.h>
#include <caneka.h>

#include "../mem/mem_chapter.c"
#include "../mem/mem.c"
#include "../mem/span/span.c"
#include "../mem/span/spanquery.c"
#include "../mem/iter.c"
#include "../mem/mem_debug.c"
#include "../core/ifc.c"
#include "./mocks.c"
#include "../debug/debug_flags.c"
/*
#include "../debug/debug_typestrings.c"
*/

int main(int argc, char *argv[]){
    MemChapter *cp = MemChapter_Make(NULL);
    if(cp != NULL){
        printf("MemChapter created successfully\n");
    }

    MemCtx *m = MemCtx_Make();
    if(m != NULL){
        printf("MemCtx created successfully\n");
    }
    i64 max = 1024;
    for(i64 i = 0; i < max; i++){
        printf("adding i64 %ld mem:%d\n", i, sizeof(i64)*(i+1)+sizeof(MemSlab)+sizeof(MemCtx));
        i64 *p = MemCtx_Alloc(m, sizeof(i64));
        memcpy(p, &i, sizeof(i64));
    }

    m->type.range++;
    for(i64 i = 0; i < max; i++){
        i64 *p = MemCtx_Alloc(m, sizeof(i64));
        memcpy(p, &i, sizeof(i64));
    }

    MemCtx_Print((Abstract *)m, 0, "MemCtx: ", COLOR_BLUE, TRUE);
    printf("\n");
    MemChapter_Print((Abstract *)cp, 0, "MemChapter: ", COLOR_YELLOW, TRUE);
    printf("\n");

    MemCtx_Free(m);
    MemCtx_Print((Abstract *)m, 0, "MemCtx: ", COLOR_BLUE, TRUE);
    printf("\n");
    MemChapter_Print((Abstract *)cp, 0, "MemChapter After Free 1: ", COLOR_YELLOW, TRUE);
    printf("\n");

    m->type.range--;
    MemCtx_Free(m);
    MemChapter_Print((Abstract *)cp, 0, "MemChapter After Free 0: ", COLOR_YELLOW, TRUE);
    printf("\n");

    exit(0);
}
