#define IS_FILE 8
#define IS_DIR 4

typedef status (*DirFunc)(MemCtx *m, Str *path, Abstract *source);
typedef status (*FileFunc)(MemCtx *m, Str *path, Str *file, Abstract *source);

status Dir_Climb(MemCtx *m, Str *path, DirFunc dir, FileFunc file, Abstract *source);
status Dir_Gather(MemCtx *m, Str *path, Span *sp);
status Dir_CheckCreate(MemCtx *m, Str *path);
status Dir_Destroy(MemCtx *m, Str *path, Access *access);
status Dir_Exists(MemCtx *m, Str *path);
