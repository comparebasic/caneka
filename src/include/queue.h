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

char *QueueFlags_ToChars(word flags);
Span *Queue_Make(MemCtx *m, GetDelayFunc getDelay);
QueueIdx *Queue_Set(SpanQuery *q, int idx, void *value, quad delayTicks);
QueueIdx *Queue_Add(SpanQuery *q, Abstract *value);
QueueIdx *Queue_Next(SpanQuery *q);
status Queue_Remove(SpanQuery *q, int idx);
