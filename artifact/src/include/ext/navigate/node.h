typedef struct node {
    Type type;
    Tokenize tk;
    i16 typeOfChild;
    i16 captureKey;
    i16 latestKey;
    i16 _;
    struct node *parent; /*util id*/
    Table *atts;
    Abstract *value;
    Abstract *child;
} Node;

Node *Node_Make(MemCh *m, word flags, Node *parent);
