enum nested_flags {
    NESTED_WITH = 1 << 8,
    NESTED_FOR =  1 << 9,
    NESTED_OUTDENT = 1 << 10,
};

typedef struct nested_state {
    Span *t; 
    int idx;
    status flags;
    word _;
    struct nested_state *prev;
} NestedState;

typedef struct nestedd {
    Type type;
    MemCtx *m;
    Span *stack;
    Span *current_tbl;
    Iter it;
} NestedD;

NestedD *NestedD_Make(MemCtx *m, Span *data_tbl);
status NestedD_Init(MemCtx *m, NestedD *nd, Span *tbl);
Abstract *NestedD_Get(NestedD *nd, Abstract *key);
status NestedD_Next(NestedD *nd);
status NestedD_Outdent(NestedD *nd);
status NestedD_For(NestedD *nd, Abstract *key);
status NestedD_With(MemCtx *m, NestedD *nd, Abstract *key);
char *NestedD_opToChars(status op);
