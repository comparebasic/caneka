typedef struct mess_item {
    Type type;
    word _;
    word tagIdx;
    Span *atts;
    Span *children; /* mess, ordtbl, hashed, strvec, or str */
    struct mess *parent;
} MessItem;

typedef struct mess {
    Type type;
    i32 count;
    MemCh *m;
    Span *tagTbl;
    MessItem *root;
} Mess;

Mess *Mess_Make(MemCh *m);
MessItem *MessItem_Make(MemCh *m, word tagIdx);
status MessItem_Append(Mess *set, MessItem *ms, Abstract *key, Abstract *a);
status MessItem_AddAtt(Mess *set, MessItem *ms, Abstract *key, Abstract *value);
