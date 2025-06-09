#define PERMS 0755 
#define PATH_BUFFLEN 1024
#define PATH_SUFFIX_MAX 128

typedef struct ioctx {
    Type type;
    MemCh *m;
    Str *root;
    Str *abs;
    Access *access;
    struct ioctx *prior;
    Span *mstore;
    Span *files;
} IoCtx;

Str *IoCtx_GetPath(MemCh *m, IoCtx *ctx, Str *path);
IoCtx *IoCtx_Make(MemCh *m, Str *root, Access *access, IoCtx *prior);
status IoCtx_Persist(MemCh *m, IoCtx *ctx);
status IoCtx_Destroy(MemCh *m, IoCtx *ctx, Access *access);
Str *IoCtx_GetMStorePath(MemCh *m, IoCtx *ctx);
Str *IoCtx_GetMStoreName(MemCh *m, IoCtx *ctx);
status IoCtx_Load(MemCh *m, IoCtx *ctx);
status IoCtx_Open(MemCh *m, IoCtx *ctx, Str *root, Access *access, IoCtx *prior);
Str *IoCtx_GetIndexName(MemCh *m, IoCtx *ctx);
status IoCtx_Init(MemCh *m, IoCtx *ctx, Str *root, Access *access, IoCtx *prior);
status IoCtx_LoadOrReserve(MemCh *m, IoCtx *ctx);
