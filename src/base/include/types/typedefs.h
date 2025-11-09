typedef status (*Func)(void *a);
typedef status (*DoFunc)(struct mem_ctx *m, void *a);
typedef boolean (*EqualFunc)(void *a, void *b); /* eq */
typedef void *(*Maker)(struct mem_ctx *m, void *a); /* mk */
typedef status (*SourceFunc)(struct mem_ctx *m, void *a, void *source);
typedef void *(*SourceMakerFunc)(struct mem_ctx *m, void *a, void *source);
typedef void *(*KeyFunc)(void *a, void *key, void *source);
typedef boolean (*EqFunc)(void *a, void *b);
typedef status (*FlagFunc)(struct mem_ctx *m, void *a, status flags);
