typedef Abstract *(*ExtFreeFunc)(struct span *m, void *v);
typedef struct external_free {
    Type type;
    ExtFreeFunc func;
    void *arg;
} ExtFree;

ExtFree *ExtFree_Make(MemCtx *m, ExtFreeFunc func, void *arg);
