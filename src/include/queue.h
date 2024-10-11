typedef struct queue_idx {
    void *item; /* slab or value */
    word flags; /* active, full, etc. */
    word nextAvailable; 
    /* delayTicks is a counter of the number of rounds the server has done
     * since it's last cleanup round, this is the minimum delay of any item
     * contained within this slab */
    quad delayTicks; 
} QueueIdx;

void *Queue_Set(Span *p, int idx, void *value, quad delayTicks);
char *QueueFlags_ToChars(word flags);
