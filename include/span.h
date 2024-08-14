#define SPAN_DEFAULT_STRIDE 16
#define SPAN_DEFAULT_STRIDE 16
#define SPAN_DEFAULT_SLOT_SIZE 1
#define SPAN_DEFAULT_ITEM_SIZE 1
#define SPAN_DEFAULT_IDX_EXTRA_SLOTS 0

enum span_flags {
    SLAB_ACTIVE = 1 << 0,
    SLAB_FULL = 1 << 1,
    SLAB_ALPHA = 1 << 2,
    SLAB_BRAVO = 1 << 3,
    SLAB_DELTA = 1 << 4,
    SLAB_ECHO = 1 << 5,
    SLAB_FOXTROT = 1 << 6,
    SLAB_GOLF = 1 << 7,
};

struct span_def;

enum span_ops {
    SPAN_OP_GET = 1,
    SPAN_OP_SET = 2,
    SPAN_OP_REMOVE = 3,
    SPAN_OP_RESERVE = 4,
};

typedef void *(*SpanDefFunc)(MemCtx *m, struct span_def *def);
typedef void *(*SpanAddrFunc)(SlabResult *sr);

typedef struct span_def {
    int stride;
    int idxStride;
    int slotSize;
    int itemSize;
    int idxExtraSlots;
    int valueHdr;
    SpanDefFunc valueSlab_Make;
    SpanDefFunc idxSlab_Make;
    SpanAddrFunc nextByIdx;
    SpanAddrFunc nextBySlot;
    SpanAddrFunc reserve;
} SpanDef;

typedef struct span {
    Type type;
    MemCtx *m;
    SpanDef *def;
	Slab *root;
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
status Span_Set(Span *p, int idx, Abstract *t);
status Span_Remove(Span *p, int idx);
status Span_ReInit(Span *p);
void *Span_Get(Span *p, int idx);
int Span_Add(Span *p, Abstract *t);
Span *Span_From(MemCtx *m, int count, ...);
void Span_Run(MemHandle *m, Span *p, Maker func, Abstract *arg);
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
void *Span_nextByIdx(SlabResult *sr);
void *Span_nextBySlot(SlabResult *sr);
void *Span_reserve(SlabResult *sr);

status Span_GetSet(SlabResult *sr, Abstract *t);
void SlabResult_Setup(SlabResult *sr, Span *p, byte op, int idx);
/* SpanDef */
byte SpanDef_GetDimNeeded(SpanDef *def, int idx);
