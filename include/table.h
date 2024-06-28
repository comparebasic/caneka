#define TABLE_MAX_DIMS 5
extern int TABLE_DIM_LOOKUPS[];

typedef struct table {
    Type type;
    Span *values;
} Table;

Abstract *Table_Get(MemCtx *m, Table *tbl, Abstract *a);
