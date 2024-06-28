#define TABLE_MAX_DIMS 5
int TABLE_DIM_LOOKUPS[5] = {15, 16*16, 16*16*16, 16*16*16*16, 16*16*16*16*16};

typedef struct table {
    Type type;
    Span *values;
} Table;

Abstract *Table_Get(MemCtx *m, Table *tbl, Abstract *a);
