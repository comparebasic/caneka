enum node_flags {
    FLAG_CHILDREN = 1 << 8,
    FLAG_CHILD = 1 << 9,
};

typedef struct node {
    Type type;
    i16 _;
    i16 captureKey;
    struct node *parent; /*util id*/
    Table *atts;
    union {
        Abstract *value;
        Span *items;
    } child;
} Node;

Node *Node_Make(MemCh *m, word flags);
