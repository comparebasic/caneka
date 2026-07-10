enum queue_flags {
    QUEUE_REVERSE = 1 << 15, /* FLAG_ITER_REVERSE */
};

typedef boolean (*QueueCritFunc)(Queue *q, void *a, void *crit);

typedef struct queue {
    Type type;
    Iter it;
    Iter critIt;
    Iter availableIt;
    QueueCritFunc func;
} Queue;

i32 Queue_Add(Queue *q, void *a);
void Queue_Set(Queue *q, i32 idx, void *a, void *crit);
void Queue_Remove(Queue *q, i32 idx);
void *Queue_GetCriteria(Queue *q, i32 idx);
void Queue_Next(Queue *q);
void Queue_Reset(Queue *q);
Queue *Queue_Make(MemCh *m);
