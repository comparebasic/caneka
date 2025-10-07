typedef struct task {
    Type type;
    i32 idx;
    util u;
    MemCh *m;
    Abstract *data;
    Abstract *source;
    Iter chainIt;
} Task;

typedef Task *(*TaskMaker)(MemCh *m, Task *task, Abstract *arg, Abstract *source);

Task *Task_Make(Span *chain, Abstract *source);
status Task_Cycle(Task *tsk);
status Task_Tumble(Task *tsk);
status Task_AddStep(Task *tsk, StepFunc func, Abstract *arg, Abstract *source, word flags);
