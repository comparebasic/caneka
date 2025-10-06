typedef struct serve_req {
    Type type;
    i32 idx;
    util u;
    MemCh *m;
    Abstract *data;
    Iter chainIt;
} Task;

Task *Task_Make(Handler *chain, Abstract *arg, Abstract *source);
status Task_Cycle(Task *req);
