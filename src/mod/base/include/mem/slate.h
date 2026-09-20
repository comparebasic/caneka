typedef i8 SlateSet[SPAN_STRIDE];

typedef struct slate {
    Type type;
    word _;
    byte __;
    i8 next;
    SlateSet set;
    Slab slab; 
} Slate;

extern SlateSet slateInitialSet;

i8 Slate_Add(Slate *slate, void *item);
void Slate_Remove(Slate *slate, i16 idx);
void Slate_Init(Slate *slate);
Slate *Slate_Make(struct mem_ctx *m);
