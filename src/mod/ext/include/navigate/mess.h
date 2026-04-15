typedef struct mess {
    Type type;
    MemCh *m;
    Node *root;
    Node *current;
    Table *nextAtts;
    void *currentValue;
    Lookup *tokenizer;
    void *source;
    struct transp_ctx *transp;
} Mess;

typedef struct mess_climber {
    Type type;
    i32 nested;
    Mess *mess;
    void *current;
    void *currentChild;
    Table *nextAtts;
} MessClimber;

status Mess_Tokenize(Mess *mess, Tokenize *tk, StrVec *v);
status Mess_Append(Mess *set, Node *node, void *a);
status Mess_AddAtt(Mess *set, Node *node, void *key, void *value);
status Mess_GetOrSet(Mess *mess, Node *node, void *a, Tokenize *tk);
status Mess_Compare(MemCh *m, Mess *a, Mess *b);
Mess *Mess_Make(MemCh *m);
