typedef struct req_crit {
    Type type;
    struct pollfd pfd;
    struct timestamp until;
} ReqCrit;

ReqCrit *ReqCrit_Make(MemCh *m);
boolean ReqCrit_Func(Queue *q, Req *req, ReqCrit *crit);
