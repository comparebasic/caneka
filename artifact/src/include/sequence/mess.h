typedef struct mess {
    Type type;
    word _;
    word tagIdx;
    struct ordered_table *atts;
    Span *children; /* mess, ordtbl, or string */
} Mess;

typedef struct mess_set {
    Type type;
    Lookup *tags;
    Span *tagTbl;
    Mess *root;
} MessSet;

MessSet *MessSet_Make(MemCtx *m);
Mess *Mess_Make(MemCtx *m, word tagIdx);
status Mess_Append(Mess *existing, Mess *ms);
status Mess_AddAtt(Mess *ms, String *key, Abstract *value);
