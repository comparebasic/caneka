/*
Span
Data Strucutre for slab allocation of segmented arrays.

This is the main work-horse data structure for storing and retrieving data, Table is biult on top of this.
*/

enum span_flags {
    SLAB_ACTIVE = 1 << 8,
    SLAB_FULL = 1 << 9,

    FLAG_SPAN_HAS_GAPS = 1 << 11,
    SPAN_ORDERED = 1 << 12,
    SPAN_RAW = 1 << 15,
};

typedef void *(*SpanAddrFunc)(struct span_query *sq);
extern i32 _increments[SPAN_MAX_DIMS];

typedef struct span {
    Type type;
    word _;
    byte _1;
    byte dims;
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

Span *Span_Make(struct mem_ctx *m);
status Span_Setup(Span *p);
status Span_Remove(Span *p, i32 idx);
status Span_Cull(Span *p, i32 count);
status Span_ReInit(Span *p);
status Span_Query(struct span_query *sq);
Span *Span_Clone(struct mem_ctx *m, Span *p);
void *Span_Set(Span *p, i32 idx, Abstract *t);
void *Span_Get(Span *p, i32 idx);
i32 Span_Add(Span *p, Abstract *t);
i32 Span_AddOrdered(Span *p, Abstract *t);
Span *Span_From(struct mem_ctx *m, i32 count, ...);
status Span_Run(struct mem_ctx *m, Span *p, DoFunc func);
status Span_Merge(Span *dest, Span *additional);
i32 Span_GetIdx(Span *p, void *a, EqualFunc eq);
Abstract *Span_Search(Span *p, void *a, EqualFunc eq);
void *Span_GetSelected(Span *p);
void *Span_ReserveNext(Span *p);
#define Span_NextIdx(p) (p->max_idx+1)

void *Span_Slab_Make(struct mem_ctx *m);
void *Span_reserve(struct span_query *sq);

status Span_GrowToNeeded(struct span_query *sq);
void *Span_SetFromQ(struct span_query *sq, Abstract *t);
void *Span_GetFromQ(struct span_query *sq);

status Span_Extend(struct span_query *sq);
status Span_GrowToNeeded(struct span_query *sq);
char **Span_ToCharArr(struct mem_ctx *m, Span *p);
status Span_Concat(Span *p, Span *add);
byte Span_GetDimNeeded(i32 idx);

/* debug */
void Span_Print(Abstract *a, cls type, char *msg, i32 color, boolean extended);
int Span_Capacity(Span *p);
Span *Span_Init(Span *p);
