typedef struct ident {
    Type type;
    Abstract *name;
    Abstract *value;
    Abstract *domain;
} Ident;

Ident *Ident_Make(MemCh *m);
Ident *Ident_FromVec(MemCh *m, StrVec *v);
status Ident_Init(MemCh *m);
status Ident_Check(void *a);
Str *Ident_NameStr(MemCh *m, Ident *ident);
Str *Ident_ValueStr(MemCh *m, Ident *ident);
Str *Ident_DomainStr(MemCh *m, Ident *ident);
