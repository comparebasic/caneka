enum queue_flags {
    QUEUE_SINGLE_IDX = 1 << 8,
};

typedef word gobits;

typedef struct queue {
    Type type;
    word _;
    word go;
    i32 localIdx;
    Abstract *value;
    Iter itemsIt;
    Span *available;
    struct {
        Span/*<QueueCrit>*/ *handlers;
        Span/*<iter>*/ *data; 
    } crit;
} Queue;

#define Queue_GetMem(q) ((q)->itemsIt.p->m)
#define Queue_Get(q) ((q)->value)
i32 Queue_GetIdx(Queue *q);
status Queue_Set(Queue *q, i32 idx, Abstract *a);
status Queue_SetCriteria(Queue *q, i32 critIdx, i32 idx, util *value);
status Queue_Next(Queue *q);
status Queue_AddHandler(Queue *q, struct queue_crit *crit);
Queue *Queue_Make(MemCh *m);
