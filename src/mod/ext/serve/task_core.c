#include <external.h>
#include <caneka.h>

TaskCore *TaskCore_Make(MemCh *m){
    TaskCore *core = MemCh_AllocOf(m, sizeof(TaskCore), TYPE_TASK_CORE);
    core->type.of = TYPE_TASK_CORE;
    return core;
}
