#define TABLE_MOD 15
enum table_levels {
    LEVEL_ONE = 0,
    LEVEL_TWO = 1,
    LEVEL_THREE = 2,
    LEVEL_FOUR = 3,
};

typedef struct table {
    Type type;
    Span *values;
    byte level;
} Table;

Abstract *Table_Get(MemCtx *m, Table *tbl, Abstract *a);
