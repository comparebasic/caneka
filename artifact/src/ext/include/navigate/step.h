enum step_flags {
    STEP_QUEUE = 1 << 8,
    STEP_ADDED = 1 << 9,
};

typedef status (*StepFunc)(Step *st, Task *tsk);

typedef struct handler {
    Type type;
    StepFunc func;
    Abstract *arg;
    Abstract *source;
} Step;

typedef Step *(*StepMaker)(MemCh *m, Task *tsk, Abstract *arg, Abstract *source);

Step *Step_Get(Step *h, Step *chain);
Step *Step_Make(MemCh *m, StepFunc func, Abstract *arg, Abstract *source);
Abstract *Step_Add(MemCh *m, Task *tsk, StepFunc func, Abstract *arg, Abstract *source);
