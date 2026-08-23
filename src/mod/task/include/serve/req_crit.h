typedef struct req_crit {
    Type type;
    struct pollfd pfd;
    struct timespec until;
} ReqCrit;

ReqCrit *ReqCrit_Make(MemCh *m);
boolean ReqCrit_Func(Queue *q, struct req *req, ReqCrit *crit);
