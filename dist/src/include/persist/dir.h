#define IS_FILE 8
#define IS_DIR 4

typedef status (*DirFunc)(MemCtx *m, String *path, Abstract *source);
typedef status (*FileFunc)(MemCtx *m, String *path, String *file, Abstract *source);

status Dir_Climb(MemCtx *m, String *path, DirFunc dir, FileFunc file, Abstract *source);
status Dir_CheckCreate(MemCtx *m, String *path);
