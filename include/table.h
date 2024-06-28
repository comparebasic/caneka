#define TABLE_MAX_DIMS 5
extern int TABLE_DIM_LOOKUPS[];

typedef struct table {
    Type type;
    Span *values;
} Table;

Abstract *Table_Get(Table *tbl, Abstract *a);
Abstract *StringTable_Get(Table *tbl, Abstract *a);
