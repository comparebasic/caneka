typedef struct ident {
    Type type;
    Abstract *name;
    Abstract *value;
    Abstract *domain;
} Ident;

Ident *Ident_Make(MemCh *m);
Ident *Ident_FromVec(MemCh *m, StrVec *v);
status Ident_Init(MemCh *m);
status Ident_Check(MemCh *m, void *a);
