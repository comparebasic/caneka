typedef void *(*ExtFreeFunc)(struct mem_ctx *m, void *v);
typedef struct external_free {
    Type type;
    ExtFreeFunc func;
    void *arg;
} ExtFree;

ExtFree *ExtFree_Make(MemCh *m, ExtFreeFunc func, void *arg);
