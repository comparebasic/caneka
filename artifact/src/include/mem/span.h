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

typedef void *slab[SPAN_STRIDE];

typedef struct span {
    Type type;
    word _;
    byte _1;
    i8 dims;
    slab *root;
    struct mem_ctx *m;
	i32 nvalues;
    i32 max_idx;
    struct {
        i32 get;
        i32 set;
        i32 selected;
        i32 available;
    } metrics;
} Span;

i32 Span_Capacity(Span *p);
char **Span_ToCharArr(struct mem_ctx *m, Span *p);
void *Span_SetFromQ(struct span_query *sq, Abstract *t);
void *Span_Set(Span *p, i32 idx, Abstract *t);
void *Span_GetFromQ(struct span_query *sq);
void *Span_Get(Span *p, i32 idx);
void *Span_SetRaw(Span *p, i32 idx, util *u);
util Span_GetRaw(Span *p, i32 idx);
i32 Span_Add(Span *p, Abstract *t);
i8 Span_GetDimNeeded(int idx);
status Span_Cull(Span *p, i32 count);
status Span_Remove(Span *p, i32 idx);
status Span_ReInit(Span *p);
status Span_Setup(Span *p);
Span *Span_Make(struct mem_ctx *m);
