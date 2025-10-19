enum task_flags {
    TASK_UPDATE_CRIT = 1 << 8,
    TASK_QUEUE = 1 << 9,
};

typedef struct task {
    Type type;
    i32 idx;
    i16 stepGuardMax;
    i16 g;
    util u;
    MemCh *m;
    Abstract *data;
    Abstract *source;
    struct task *parent;
    Iter chainIt;
} Task;

extern Table *TaskErrorHandlers;

typedef status (*TaskPopulate)(MemCh *m, Task *task, Abstract *arg, Abstract *source);

Task *Task_Make(Span *chain, Abstract *source);
status Task_Cycle(Task *tsk);
status Task_Tumble(Task *tsk);
status Task_AddDataStep(Task *tsk, StepFunc func, Abstract *arg, Abstract *data, Abstract *source, word flags);
status Task_AddStep(Task *tsk, StepFunc func, Abstract *arg, Abstract *source, word flags);
status Task_Free(Step *st, Task *tsk);
status Task_ResetChain(Task *tsk);

status Task_Init(MemCh *m);
