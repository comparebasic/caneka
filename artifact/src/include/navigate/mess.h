typedef struct mess {
    Type type;
    MemCh *m;
    Node *root;
    Abstract *source;
} Mess;

status Mess_Append(Mess *set, Node *node, Abstract *a);
status Mess_AddAtt(Mess *set, Node *node, Abstract *key, Abstract *value);
Mess *Mess_Make(MemCh *m);
