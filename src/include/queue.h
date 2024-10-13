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
    Span span;
    /* */
    SpanQuery sr;
    GetDelayFunc getDelay;
} Queue;

void *Queue_Set(Span *p, int idx, void *value, quad delayTicks);
char *QueueFlags_ToChars(word flags);
void *Queue_Add(Queue *q, Abstract *value);
void *Queue_Make(MemCtx *m, GetDelayFunc getDelay);
void *Queue_Remove(Queue *q, int idx);
