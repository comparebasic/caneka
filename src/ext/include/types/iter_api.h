extern struct lookup * IterApiLookup;

typedef struct inst_iter_api {
    Type type;
    IterFunc next;
    IterFunc prev;
    IterGetFunc get;
    Abstract *source;
} IterApi;

status IterApi_Init(MemCh *m);
IterApi *IterApi_Make(MemCh *m, IterFunc next, IterFunc prev, IterGetFunc get);
