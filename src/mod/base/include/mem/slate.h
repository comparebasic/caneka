typedef struct slate {
    Type type;
    struct {
        i8 max;
        i8 min;
        i8 nextInQueue;
        i8 count;
    } idx;
    i8 queue[SPAN_STRIDE];
    void *slots[SPAN_STRIDE];
} Slate;

extern i8 slateInitialSet[SPAN_STRIDE];

i8 Slate_Add(Slate *slate, void *item);
void Slate_Remove(Slate *slate, i16 idx);
void Slate_Init(Slate *slate);
Slate *Slate_Make(struct mem_ctx *m);
