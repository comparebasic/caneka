#include <external.h>
#include <caneka.h>

/* 
This is a fomula to take parts of the has for the coordinates of different levels of the
has table, each represented by a span with different dimensions
*/
static int getTieredHash(util hash, byte level){
    return (byte) ((hash >> (level*4)) & TABLE_MOD);
}

Abstract *Table_Get(MemCtx *m, Table *tbl, Abstract *a){
    Hashed *h = Hashed_Make(m, a);
    if(h != NULL){
        byte b = getTieredHash(h->id, 0);
        Bits_Print(&b, sizeof(byte), "Hash level 0", COLOR_CYAN);
        printf("\n");
    }
    return NULL;
}
