typedef struct task {
    Type type;
    i32 idx;
    util u;
    MemCh *m;
    Abstract *data;
    Iter chainIt;
} Task;

Task *Task_Make(Span *chain);
status Task_Cycle(Task *tsk);
status Task_Tumble(Task *tsk);
status Task_AddStep(Task *tsk, StepFunc func, Abstract *arg, Abstract *source);
