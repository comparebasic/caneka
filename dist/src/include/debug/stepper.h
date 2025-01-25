enum stepper_flags {
   STEPPER_BYPASS = 1 << 8, 
   STEPPER_END = 1 << 9, 
};

extern word StepperFlags;

void Stepper(Abstract *a);
