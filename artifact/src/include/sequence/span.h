/*
Span
Data Strucutre for slab allocation of segmented arrays.

This is the main work-horse data structure for storing and retrieving data, Table is biult on top of this.

related: core/span.c
related: include/slab.h
*/
#define SPAN_SLOT_BYTES sizeof(void *)

#define SPAN_VALUE_SIZE(def) (sizeof(Abstract *)*def->stride*def->slotSize)
#define SPAN_IDX_SIZE(def) (sizeof(Abstract *)*(def->idxSize)*def->idxStride)

#define SPAN_STRIDE 16

enum span_flags {
    SLAB_ACTIVE = 1 << 8,
    SLAB_FULL = 1 << 9,

    FLAG_SPAN_HAS_GAPS = 1 << 11,
    SPAN_ORDERED = 1 << 12,
    SPAN_RAW = 1 << 15,
};

typedef void *(*SpanAddrFunc)(struct span_query *sq);

typedef struct span {
    Type type;
    byte _;
    byte ptrSlot;
    byte slotSize;
    byte dims;
    void *root;
    MemCtx *m;
	int nvalues;
    int max_idx;
    struct {
        int get;
        int set;
        int selected;
        int available;
    } metrics;
} Span;

Span *Span_Make(MemCtx* m);
Span *Span_Setup(Span *p);
status Span_Remove(Span *p, int idx);
status Span_Cull(Span *p, int count);
status Span_ReInit(Span *p);
status Span_Query(struct span_query *sq);
Span *Span_Clone(MemCtx *m, Span *p);
void *Span_Set(Span *p, int idx, Abstract *t);
void *Span_Get(Span *p, int idx);
int Span_Add(Span *p, Abstract *t);
int Span_AddOrdered(Span *p, Abstract *t);
Span *Span_From(MemCtx *m, int count, ...);
status Span_Run(MemCtx *m, Span *p, DoFunc func);
status Span_Merge(Span *dest, Span *additional);
int Span_GetIdx(Span *p, void *a, EqualFunc eq);
Abstract *Span_Search(Span *p, void *a, EqualFunc eq);
void *Span_GetSelected(Span *p);
void *Span_ReserveNext(Span *p);
#define Span_NextIdx(p) (p->max_idx+1)

void *Span_Slab_Make(MemCtx *m);
void *Span_reserve(struct span_query *sq);

status Span_GrowToNeeded(struct span_query *sq);
void *Span_SetFromQ(struct span_query *sq, Abstract *t);
void *Span_GetFromQ(struct span_query *sq);

status Span_Extend(struct span_query *sq);
status Span_GrowToNeeded(struct span_query *sq);
char **Span_ToCharArr(MemCtx *m, Span *p);
status Span_Concat(Span *p, Span *add);
byte Span_GetDimNeeded(int idx);

/* debug */
void Span_Print(Abstract *a, cls type, char *msg, int color, boolean extended);
int Span_availableByDim(int dims, int stride);
int Span_Capacity(Span *p);
Span *Span_Init(Span *p);
