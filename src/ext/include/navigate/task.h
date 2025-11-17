#define TASK_TUMPLE_MAX 1024

enum task_flags {
    TASK_UPDATE_CRIT = 1 << 8,
    TASK_QUEUE = 1 << 9,
    TASK_CHILD = 1 << 10,
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

typedef status (*TaskPopulate)(MemCh *m, Task *task, void *arg, void *source);

Task *Task_Make(Span *chain, void *source);
status Task_Cycle(Task *tsk);
status Task_Tumble(Task *tsk);
status Task_AddDataStep(Task *tsk,
    StepFunc func, void *arg, void *data, void *source, word flags);
status Task_AddStep(Task *tsk,
    StepFunc func, void *arg, void *source, word flags);
status Task_Free(Step *st, Task *tsk);
status Task_ResetChain(Task *tsk);

status Task_Init(MemCh *m);
