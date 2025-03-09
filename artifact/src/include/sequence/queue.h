typedef status (*SetBitFunc)(Queue *q, Abstract *item, Abstract *source);
typedef int (*SkipSlabFunc)(Abstract *source, int idx);

enum queue_flags {
   QUEUE_INCOMING = 1 << 9, 
   QUEUE_RESPONDING = 1 << 10, 
};

typedef struct queue {
    Type type;
    Span *span;
    Span *available;
    Span *bits;
    Iter it;
    SetBitFunc setBits;
    Abstract *source;
} Queue;

status Queue_Init(MemCtx *m, Queue *q, GetDelayFunc getDelay);

Queue *Queue_Make(MemCtx *m, GetDelayFunc getDelay);
i32 Queue_Add(Queue *q, Abstract *value);
Abstract *Queue_Next(Queue *q);
status Queue_Remove(Queue *q, int idx);
