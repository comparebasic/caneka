enum span_flags {
    FLAG_SPAN_INLINE_SLOTS = 1 << 8,
    FLAG_SPAN_HAS_GAPS = 1 << 9,
    FLAG_SPAN_ORDERED = 1 << 10,
    FLAG_SPAN_RAW = 1 << 11,
    FLAG_SPAN_TABLE = 1 << 12,
};

typedef util *slab[SPAN_STRIDE];
extern i32 dim_max_idx[SPAN_MAX_DIMS+1];

typedef struct span {
    Type type;
    byte _;
    i8 dims;
    i16 memLevel;
    struct mem_ctx *m;
    slab *root;
	i32 nvalues;
    i32 max_idx;
} Span;

status Span_Set(Span *p, i32 idx, void *t);
void *Span_Get(Span *p, i32 idx);
status Span_Remove(Span *p, i32 idx);
status Span_Setup(Span *p);
Span *Span_Make(struct mem_ctx *m);
boolean Span_IsBlank(Span *p);
