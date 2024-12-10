/*
Span
Data Strucutre for slab allocation of segmented arrays.

This is the main work-horse data structure for storing and retrieving data, Table is biult on top of this.

related: core/span.c
related: include/slab.h
*/
#define SPAN_SLOT_BYTES sizeof(void *)
#define SPAN_DEFAULT_STRIDE 16
#define SPAN_DEFAULT_STRIDE 16
#define SPAN_DEFAULT_SLOT_SIZE 1
#define SPAN_DEFAULT_IDX_SIZE 1
#define SPAN_DEFAULT_ITEM_SIZE 1
#define SPAN_DEFAULT_IDX_EXTRA_SLOTS 0
#define SPAN_DEFAULT_HDR 0

#define SPAN_VALUE_SIZE(def) (sizeof(Abstract *)*def->stride*def->slotSize)
#define SPAN_IDX_SIZE(def) (sizeof(Abstract *)*(def->idxSize)*def->idxStride)

enum span_flags {
    SLAB_ACTIVE = 1 << 8,
    SLAB_FULL = 1 << 10,
    FLAG_SPAN_HAS_GAPS = 1 << 11,
};

typedef void *(*SpanDefFunc)(MemCtx *m, struct span_def *def);
typedef void *(*SpanAddrFunc)(struct span_query *sq);

typedef struct span {
    Type type;
    MemCtx *m;
    SpanDef *def;
    /* Queue Idx */
	void *root;
    word flags;
    word count;
    quad  delayTicks;
    /* end QueuIdx */
    byte dims;
	int nvalues;
    int max_idx;
    struct {
        int get;
        int set;
        int selected;
    } metrics;
} Span;

Span* Span_Make(MemCtx* m, cls type);
Span* Span_MakeInline(MemCtx* m, cls type, int itemSize);
status Span_Remove(Span *p, int idx);
status Span_Cull(Span *p, int count);
status Span_ReInit(Span *p);
status Span_Query(struct span_query *sq);
Span *Span_Clone(MemCtx *m, Span *p);
void *Span_Set(Span *p, int idx, Abstract *t);
void *Span_Get(Span *p, int idx);
int Span_Add(Span *p, Abstract *t);
Span *Span_From(MemCtx *m, int count, ...);
status Span_Run(MemCtx *m, Span *p, DoFunc func);
status Span_Merge(Span *dest, Span *additional);
int Span_GetIdx(Span *p, void *a, EqualFunc eq);
Abstract *Span_Search(Span *p, void *a, EqualFunc eq);
void *Span_GetSelected(Span *p);
void *Span_ReserveNext(Span *p);
#define Span_NextIdx(p) (p->max_idx+1)

/* span type makers */
SpanDef *Span4x16_MakeDef();
SpanDef *Span16_MakeDef();
SpanDef *Span16x32m_MakeDef();
SpanDef *Span4kx32m_MakeDef();
SpanDef *SpanString_MakeDef();

void *Span_valueSlab_Make(MemCtx *m, SpanDef *def);
void *Span_idxSlab_Make(MemCtx *m, SpanDef *def);
void *Span_reserve(struct span_query *sq);

status Span_GrowToNeeded(struct span_query *sq);
void *Span_SetFromQ(struct span_query *sq, Abstract *t);
void *Span_GetFromQ(struct span_query *sq);

status Span_Extend(struct span_query *sq);
status Span_GrowToNeeded(struct span_query *sq);

/* SpanDef */
SpanDef *SpanDef_Clone(MemCtx *m, SpanDef *_def);
byte SpanDef_GetDimNeeded(SpanDef *def, int idx);

/* debug */
void Span_Print(Abstract *a, cls type, char *msg, int color, boolean extended);
int Span_availableByDim(int dims, int stride, int idxStride);
int Span_Capacity(Span *p);
