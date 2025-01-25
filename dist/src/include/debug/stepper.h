enum stepper_flags {
   STEPPER_BYPASS = 1 << 8, 
   STEPPER_END = 1 << 9, 
};

typedef struct step {
    Type type;
    bypassFor;
    Abstract *a;
    String *desc;
    String *num_s;
    Maker Func; /* a callback for updating the prompt */
} Step;

extern word StepperFlags;

void Stepper(Abstract *a);
status Stepper_Make(MemCtx *m, Abstract *a, word flags, String *desc, String *num_s);
status Stepper_BypassFor(Step *st, int num);
static status Steps_Init(MemCtx *m);
