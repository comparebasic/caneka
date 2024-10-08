typedef struct queue_idx {
    void *item; /* slab or value */
    word flags; /* active, full, etc. */
    word _; /* not yet used */
    /* delayTicks is a counter of the number of rounds the server has done
     * since it's last cleanup round, this is the minimum delay of any item
     * contained within this slab */
    quad delayTicks; 
} QueueIdx;
