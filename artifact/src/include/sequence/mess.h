typedef struct mess {
    Type type;
    word _;
    word tagIdx;
    Span *atts;
    Span *children; /* mess, ordtbl, hashed, strvec, or str */
    struct mess *parent;
} Mess;

typedef struct mess_set {
    Type type;
    MemCtx *m;
    Span *tagTbl;
    Mess *root;
} MessSet;

MessSet *MessSet_Make(MemCtx *m);
Mess *Mess_Make(MemCtx *m, word tagIdx);
status Mess_Append(MessSet *set, Mess *ms, Abstract *key, Abstract *a);
status Mess_AddAtt(MessSet *set, Mess *ms, Abstract *key, Abstract *value);
