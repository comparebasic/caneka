#define IS_FILE 8
#define IS_DIR 4

enum dir_selector_flags {
    DIR_SELECTOR_MTIME_ALL = 1 << 8,
};

typedef struct dir_selector {
    Type type;
    Str *ext;
    Span *dest;
    struct timespec tm;
} DirSelector;

extern Span *FilePathSep;

typedef status (*DirFunc)(MemCh *m, Str *path, void *source);
typedef status (*FileFunc)(MemCh *m, Str *path, Str *file, void *source);

status Dir_Destroy(MemCh *m, Str *path);
status Dir_Gather(MemCh *m, Str *path, Span *sp);
status Dir_Exists(MemCh *m, Str *path);
status Dir_Climb(MemCh *m, Str *path, DirFunc dir, FileFunc file, void *source);
status Dir_CheckCreate(MemCh *m, Str *path);
status Dir_Mk(MemCh *m, Str *path);
status Dir_Rm(MemCh *m, Str *path);

DirSelector *DirSelector_Make(MemCh *m, Span *dest, word flags);
