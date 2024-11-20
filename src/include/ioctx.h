#define PERMS 0755 
#define PATH_BUFFLEN 1024
#define PATH_SUFFIX_MAX 128
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
    struct mem_local *mstore;
    Span *files;
} IoCtx;

String *IoCtx_GetPath(MemCtx *m, IoCtx *ctx, String *path);
IoCtx *IoCtx_Make(MemCtx *m, String *root, Access *access, IoCtx *prior);
status IoCtx_Persist(MemCtx *m, IoCtx *ctx);
status IoCtx_Destroy(MemCtx *m, IoCtx *ctx, Access *access);
String *IoCtx_GetMStorePath(MemCtx *m, IoCtx *ctx);
String *IoCtx_GetMStoreName(MemCtx *m, IoCtx *ctx);
status IoCtx_Load(MemCtx *m, IoCtx *ctx);
