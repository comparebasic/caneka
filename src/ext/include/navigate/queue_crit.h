/*(SPAN_STRIDE*SPAN_STRIDE)*/
#define CRIT_SLAB_STRIDE 64
/*(SPAN_STRIDE*SPAN_STRIDE)-1*/
#define CRIT_SLAB_MASK 63

enum queue_crit_flags {
    QUEUE_CRIT_REQUIRED = 1 << 11,
    QUEUE_CRIT_UTIL = 1 << 12,
    QUEUE_CRIT_PFD = 1 << 13,
};

typedef gobits (*QueueFunc)(struct queue_crit *crit, util *values);

typedef struct queue_crit {
    Type type;
    util u;
    QueueFunc func;
    Span *data;
} QueueCrit;

QueueCrit *QueueCrit_Make(MemCh *m, QueueFunc func, word flags);
