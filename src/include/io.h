#define PERMS 577
typedef struct access {
    Type type;
    String *owner;
    Span *groups;
} Access;

typedef struct ioctx {
    Type type;
    MemCtx *m;
    String *root;
    String *abs;
    Access *access;
    struct ioctx *prior;
    Span *tbl;
    Span *files;
    Iter *it;
} IoCtx;

String *IoCtx_GetPath(MemCtx *m, IoCtx *ctx, String *path_s);
IoCtx *IoCtx_Make(MemCtx *m, String *root, Access *access);
IoCtx *IoCtx_Persist(MemCtx *m, IoCtx *ctx);
