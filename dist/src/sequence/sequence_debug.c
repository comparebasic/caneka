#include <external.h>
#include <caneka.h>

void TableChain_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    TableChain *tch = (TableChain *)as(a, TYPE_TABLE_CHAIN);
    printf("\x1b[%dmTChain<", color);
    if(extended){
        while(tch != NULL){
            printf("\n    ");
            Debug_Print((Abstract *)tch->tbl, 0, "tbl: ", color, extended);
            tch = tch->next;
        }
    }
    if(extended){
        while(tch != NULL){
            printf("\x1b[%dmtbl(%d)", color, tch->tbl != NULL ? tch->tbl->nvalues : 0);
            if(tch != NULL){
                printf("\x1b[%dm, ", color);
            }
        }
    }
    printf("\x1b[%dm>\x1b[0m", color);

}
