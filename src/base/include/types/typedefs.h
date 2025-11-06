typedef status (*Func)(Abstract *a);
typedef status (*DoFunc)(struct mem_ctx *m, Abstract *a);
typedef boolean (*EqualFunc)(Abstract *a, void *b); /* eq */
typedef Abstract *(*Maker)(struct mem_ctx *m, Abstract *a); /* mk */
typedef status (*SourceFunc)(struct mem_ctx *m, Abstract *a, Abstract *source);
typedef Abstract *(*SourceMakerFunc)(struct mem_ctx *m, Abstract *a, Abstract *source);
typedef Abstract *(*KeyFunc)(Abstract *a, Abstract *key, Abstract *source);
typedef boolean (*EqFunc)(Abstract *a, Abstract *b);
typedef status (*FlagFunc)(struct mem_ctx *m, Abstract *a, status flags);
