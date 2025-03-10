#include <external.h>
#include <caneka.h>

#include "../mem/mem_chapter.c"
#include "../mem/mem.c"
#include "../mem/span/slab.c"
#include "../mem/span/span.c"
#include "../mem/span/spanquery.c"
#include "../mem/iter.c"
#include "../mem/mem_debug.c"
#include "../error_mocks.c"
#include "../debug_mocks.c"
#include "../debug/debug_flags.c"
#include "../debug/debug_typestrings.c"

int main(int argc, char *argv[]){
    MemChapter *cp = MemChapter_Make(NULL);
    if(cp != NULL){
        printf("MemChapter created successfully\n");
    }

    MemCtx *m = MemCtx_Make();
    i64 max = 1024;
    for(i64 i = 0; i < max; i++){
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
