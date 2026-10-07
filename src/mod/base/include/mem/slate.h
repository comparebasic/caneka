typedef struct slate {
    Type type;
    struct {
        i8 max;
        i8 min;
        i8 nextInQueue;
        i8 count;
    } idx;
    i8 queue[SPAN_STRIDE];
    status flags[SPAN_STRIDE];
    void *slots[SPAN_STRIDE];
} Slate;

extern i8 slateInitialSet[SPAN_STRIDE];

void *Slate_Get(Slate *slate, i8 idx);
i8 Slate_Add(Slate *slate, void *item);
i8 Slate_Enqueue(Slate *slate, void *item);
i8 Slate_Insert(Slate *slate, i8 idx, void *item);
void Slate_Remove(Slate *slate, i8 idx);
void Slate_Init(Slate *slate);
Slate *Slate_Make(struct mem_ctx *m);
