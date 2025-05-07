typedef struct mess {
    Type type;
    MemCh *m;
    Node *root;
    Node *current;
    Abstract *currentValue;
    Lookup *tokenizer;
    Abstract *source;
} Mess;

typedef struct mess_climber {
    Type type;
    i32 nested;
    Mess *mess;
    Abstract *current;
} MessClimber;

status Mess_Tokenize(Mess *mess, Tokenize *tk, StrVec *v);
status Mess_Append(Mess *set, Node *node, Abstract *a);
status Mess_AddAtt(Mess *set, Node *node, Abstract *key, Abstract *value);
status Mess_GetOrSet(Mess *mess, Node *node, Abstract *a, cls typeOf);
Mess *Mess_Make(MemCh *m);
