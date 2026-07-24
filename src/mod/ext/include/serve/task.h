enum task_properties {
    TASK_CORE = 0,
    TASK_DEF = 1,
    TASK_IO = 2,
    TASK_CONN = 3,
    TASK_CAPSULE = 4,
    TASK_DOMAIN = 5,
    TASK_PROTOCOL = 6,
    TASK_SOURCE = 7,
};

status Task_ClsInit(MemCh *m);
