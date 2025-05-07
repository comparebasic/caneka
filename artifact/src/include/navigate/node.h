typedef struct node {
    Type type;
    i16 typeOfChild;
    i16 captureKey;
    struct node *parent; /*util id*/
    Table *atts;
    Abstract *child;
} Node;

Node *Node_Make(MemCh *m, word flags, Node *parent);
