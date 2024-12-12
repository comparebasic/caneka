typedef struct mess {
    Type type;
    String *name;
    Span *atts;
    String *body;
    struct mess *parent;
    struct mess *firstChild;
    struct mess *next;
} Mess;

Mess *Mess_Make(MemCtx *m);
status Mess_Append(Mess *existing, Mess *ms);
