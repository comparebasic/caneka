/*
Span
Data Strucutre for slab allocation of segmented arrays.

This is the main work-horse data structure for storing and retrieving data, Table is biult on top of this.
*/

enum span_flags {
    FLAG_SPAN_INLINE_SLOTS = 1 << 8,
    FLAG_SPAN_HAS_GAPS = 1 << 9,
    FLAG_SPAN_ORDERED = 1 << 10,
    FLAG_SPAN_RAW = 1 << 11,
    FLAG_SPAN_TABLE = 1 << 12,
};

typedef util *slab[SPAN_STRIDE];
extern i32 _increments[SPAN_MAX_DIMS+1];

typedef struct span {
    Type type;
    word _;
    byte _1;
    i8 dims;
    slab *root;
    struct mem_ctx *m;
	i32 nvalues;
    i32 max_idx;
} Span;

status Span_Set(Span *p, i32 idx, Abstract *t);
void *Span_Get(Span *p, i32 idx);
status Span_Remove(Span *p, i32 idx);
status Span_Setup(Span *p);
Span *Span_Make(struct mem_ctx *m);
