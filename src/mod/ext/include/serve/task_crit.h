typedef struct req_crit {
    Type type;
    struct pollfd pfd;
    struct timespec until;
} TaskCrit;

TaskCrit *TaskCrit_Make(MemCh *m);
boolean TaskCrit_Func(Queue *q, struct req *req, TaskCrit *crit);
