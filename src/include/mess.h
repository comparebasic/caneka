typedef struct mess {
    Type type;
    Hashed *value;
    Hashed *atts;
    String *body;
    struct mess *next;
    struct mess *parent;
    struct mess *firstChild;
} Mess;

Mess *Mess_Make(MemCtx *m);
status Mess_Append(Mess *existing, Mess *ms);
