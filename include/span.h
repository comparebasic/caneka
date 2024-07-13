enum span_flags {
    FLAG_SPAN_AUTOEXPAND = 1 << 16,
};

enum span_ops {
    SPAN_OP_GET = 1,
    SPAN_OP_SET = 2,
    SPAN_OP_REMOVE = 3,
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
    /*
    Resolver *rslv;
    */
} Span;

Span* Span_Make(MemCtx* m);
Span* Span_MakeInline(MemCtx* m, cls type, int itemSize);
status Span_Set(Span *p, int idx, Abstract *t);
status Span_Remove(Span *p, int idx);
void *Span_Get(Span *p, int idx);
int Span_Add(Span *p, Abstract *t);
Span *Span_From(MemCtx *m, int count, ...);
#define Span_NextIdx(p) (p->max_idx+1)
