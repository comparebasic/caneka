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
    Span *root;
    NestedState *current;
    Span *stack;
    Span *current_tbl;
    Iter it;
} NestedD;

NestedD *NestedD_Make(MemCtx *m, Span *data_tbl);
Abstract *NestedD_Get(NestedD *nd, Abstract *key);
status NestedD_Next(NestedD *nd);
status NestedD_Outdent(MemCtx *m, NestedD *nd);
status NestedD_For(MemCtx *m, NestedD *nd, Abstract *key);
status NestedD_With(MemCtx *m, NestedD *nd, Abstract *key);
char *NestedD_opToChars(status op);
