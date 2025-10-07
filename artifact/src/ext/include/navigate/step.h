enum step_flags {
    STEP_IO_IN = 1 << 8,
    STEP_IO_OUT = 1 << 9,
};

typedef status (*StepFunc)(struct step *st, struct task *tsk);

typedef struct step {
    Type type;
    StepFunc func;
    Abstract *arg;
    Abstract *source;
    Abstract *data;
} Step;

Step *Step_Make(MemCh *m, StepFunc func, Abstract *arg, Abstract *source, word flags);
status Step_Add(MemCh *m, 
    struct task *tsk, StepFunc func, Abstract *arg, Abstract *source);
