enum span_ops {
    SPAN_OP_GET = 1,
    SPAN_OP_SET = 2,
    SPAN_OP_REMOVE = 3,
    SPAN_OP_RESERVE = 4,
};

typedef struct span {
    Type type;
    MemCtx *m;
	Slab *slab;
	int nvalues;
    int max_idx;
    cls itemType;
    byte slotSize;
    byte itemSize;
	byte dims;
    struct {
        int get;
        int set;
        int selected;
    } metrics;
} Span;

Span* Span_Make(MemCtx* m);
Span* Span_MakeMini(MemCtx* m);
Span* Span_MakeInline(MemCtx* m, cls type, int itemSize);
status Span_Set(Span *p, int idx, Abstract *t);
status Span_Remove(Span *p, int idx);
status Span_ReInit(Span *p);
void *Span_Get(Span *p, int idx);
int Span_Add(Span *p, Abstract *t);
Span *Span_From(MemCtx *m, int count, ...);
void Span_Run(MemCtx *m, Span *p, Maker func, Abstract *arg);
status Span_Merge(Span *dest, Span *additional);
int Span_GetIdx(Span *p, void *a, EqualFunc eq);
Abstract *Span_Search(Span *p, void *a, EqualFunc eq);
void *Span_GetSelected(Span *p);
void *Span_ReserveNext(Span *p);
#define Span_NextIdx(p) (p->max_idx+1)
