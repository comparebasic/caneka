#define IS_FILE 8
#define IS_DIR 4

typedef status (*DirFunc)(MemCh *m, Str *path, Abstract *source);
typedef status (*FileFunc)(MemCh *m, Str *path, Str *file, Abstract *source);

status Dir_Destroy(MemCh *m, Str *path, Access *access);
status Dir_Gather(MemCh *m, Str *path, Span *sp);
status Dir_Exists(MemCh *m, Str *path);
status Dir_Climb(MemCh *m, Str *path, DirFunc dir, FileFunc file, Abstract *source);
status Dir_CheckCreate(MemCh *m, Str *path);
