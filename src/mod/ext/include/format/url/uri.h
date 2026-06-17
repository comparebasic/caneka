typedef struct uri {
    Type type;
    StrVec *v;
    StrVec *path;
    StrVec *host;
    Str *proto;
    Iter queryIt;
} Uri;

Uri *Uri_Make(MemCh *m, StrVec *v);
