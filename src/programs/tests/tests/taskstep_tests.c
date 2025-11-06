#include <external.h>
#include <caneka.h>

static status funcThree(Step *st, Task *tsk){
    status r = READY;
    Abstract *args[2];
    Single *count = (Single *)tsk->source;
    args[0] = (Abstract *)count;
    args[1] = NULL;
    r |= Test(count->type.of == TYPE_WRAPPED_I32 && count->val.i == 2,
        "Source str equals tsk->source with expected count $", args);
    count->val.i++;
    st->type.state |= SUCCESS;
    tsk->type.state |= MORE;
    return r;
}

static status funcTwo(Step *st, Task *tsk){
    status r = READY;
    Abstract *args[2];
    Single *count = (Single *)tsk->source;
    args[0] = (Abstract *)count;
    args[1] = NULL;
    r |= Test(count->type.of == TYPE_WRAPPED_I32 && count->val.i == 1,
        "Source str equals tsk->source with expected count $", args);
    count->val.i++;
    r |= Task_AddStep(tsk, funcThree, NULL, NULL, ZERO);
    st->type.state |= SUCCESS;
    return r;
}

static status funcOne(Step *st, Task *tsk){
    status r = READY;
    Abstract *args[2];
    Single *count = (Single *)tsk->source;
    args[0] = (Abstract *)count;
    args[1] = NULL;
    r |= Test(count->type.of == TYPE_WRAPPED_I32 && count->val.i == 0,
        "Source str equals tsk->source with expected count $", args);
    count->val.i++;
    st->type.state |= SUCCESS;
    tsk->type.state |= MORE;
    return r;
}

status TaskStep_Tests(MemCh *gm){
    DebugStack_Push(NULL, 0);
    MemCh *m = MemCh_Make();
    status r = READY;
    Abstract *args[2];

    Single *count = I32_Wrapped(m, 0);
    Task *tsk = Task_Make(Span_Make(m), (Abstract *)count);
    Task_AddStep(tsk, funcTwo, NULL, NULL, ZERO);
    Task_AddStep(tsk, funcOne, NULL, NULL, ZERO);
    Task_Tumble(tsk);

    args[0] = (Abstract *)tsk;
    args[1] = NULL;
    r |= Test(tsk->type.state & SUCCESS, "Task finishes successfully", args);

    MemCh_Free(m);
    DebugStack_Pop();
    return r;
}
