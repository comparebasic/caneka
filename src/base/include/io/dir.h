#define IS_FILE 8
#define IS_DIR 4

enum dir_selector_flags {
    DIR_SELECTOR_MTIME_ALL = 1 << 8,
    DIR_SELECTOR_MTIME_LOWEST = 1 << 9,
    DIR_SELECTOR_NODIRS = 1 << 10,
    DIR_SELECTOR_FILTER = 1 << 11,
    DIR_SELECTOR_FILTER_DIRS = 1 << 12,
    DIR_SELECTOR_INVERT = 1 << 13,
};

typedef struct dir_selector {
    Type type;
    Str *ext;
    Span *dest;
    Span *meta; /* Table */
    struct timespec time;
    SourceFunc func;
    void *source;
} DirSel;

extern Span *FilePathSep;

typedef status (*DirFunc)(MemCh *m, Str *path, void *source);
typedef status (*FileFunc)(MemCh *m, Str *path, Str *file, void *source);

status Dir_Destroy(MemCh *m, Str *path);
status Dir_Gather(MemCh *m, Str *path, Span *sp);
status Dir_GatherSel(MemCh *m, Str *path, DirSel *sel);
status Dir_GatherFilterDir(MemCh *m, Str *path, DirSel *sel);
DirSel *Dir_GatherByExt(MemCh *m, Str *path, Span *sp, Span *exts);
status Dir_Exists(MemCh *m, Str *path);
status Dir_Climb(MemCh *m, Str *path, DirFunc dir, FileFunc file, void *source);
status Dir_CheckCreate(MemCh *m, Str *path);
status Dir_Mk(MemCh *m, Str *path);
status Dir_Rm(MemCh *m, Str *path);

DirSel *DirSel_Make(MemCh *m, Str *ext, Span *dest, word flags);
