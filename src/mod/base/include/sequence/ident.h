typedef struct ident {
    Type type;
    Str *name;
    Str *value;
    Str *domain;
}

Ident *Ident_Make(MemCh *m);
Ident *Ident_FromVec(MemCh *m
