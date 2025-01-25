enum stepper_flags {
   STEPPER_BYPASS = 1 << 8, 
   STEPPER_END = 1 << 9, 
};

typedef struct step {
    Type type;
    int bypassFor;
    Abstract *a;
    String *desc;
    String *num_s;
    Exch Func; /* a callback for updating the prompt */
} Step;

void Stepper(MemCtx *m, Abstract *a);
status Steps_Init(MemCtx *m);
Step *Stepper_Make(MemCtx *m, Abstract *a, word flags, String *desc, String *num_s);
status Stepper_BypassFor(Step *st, int num);
Exch Stepper_GetDescFunc(word type);

/* desc Exch funcs */
Abstract *Roebling_StepDesc(MemCtx *m, Abstract *a, Abstract *b);
