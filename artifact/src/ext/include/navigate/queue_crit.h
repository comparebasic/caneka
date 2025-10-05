enum queue_crit_flags {
    QUEUE_CRIT_REQUIRED = 1 << 11,
};

typedef gobits (*QueueFunc)(struct queue_crit *crit, Abstract **items, util *values);

typedef struct queue_crit {
    Type type;
    QueueFunc func;
    Span *data;
} QueueCrit;

QueueCrit *QueueCrit_Make(MemCh *m, QueueFunc func, word flags);
