enum step_flags {
    STEP_QUEUE = 1 << 8,
    STEP_ADDED = 1 << 9,
};

typedef status (*StepFunc)(struct step *st, struct task *tsk);

typedef struct step {
    Type type;
    StepFunc func;
    Abstract *arg;
    Abstract *source;
} Step;

Step *Step_Make(MemCh *m, StepFunc func, Abstract *arg, Abstract *source);
status Step_Add(MemCh *m, 
    struct task *tsk, StepFunc func, Abstract *arg, Abstract *source);
