typedef quad (*GetDelayFunc)(status r);

typedef struct queue_idx {
    void *item; /* slab or value */
    word flags; /* active, full, etc. */
    word count; 
    /* delayTicks is a counter of the number of rounds the server has done
     * since it's last cleanup round, this is the minimum delay of any item
     * contained within this slab */
    quad delayTicks; 
} QueueIdx;

typedef struct queue {
    Type type;
    Span *span;
    SpanQuery current;
    SpanQuery available;
    GetDelayFunc getDelay;
} Queue;

status Queue_Init(MemCtx *m, Queue *q, GetDelayFunc getDelay);

char *QueueFlags_ToChars(word flags);
Span *Queue_Make(MemCtx *m, GetDelayFunc getDelay);
QueueIdx *Queue_Set(Queue *q, int idx, void *value, quad delayTicks);
QueueIdx *Queue_Add(Queue *q, Abstract *value);
QueueIdx *Queue_Next(Queue *q);
status Queue_SetDelay(SpanQuery *sq, quad delayTicks);
status Queue_Remove(Queue *q, int idx);
