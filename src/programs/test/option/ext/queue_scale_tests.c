#include <external.h>
#include <caneka.h>
#include <test_module.h>


static boolean Queue_timeFunc(Queue *q, Str *item, ApproxTime *crit){
    ApproxTime *delta = &q->time.delta;
    return (delta->type.state & UPPER_FLAGS) &&     
            (delta->type.state & UPPER_FLAGS) >= (crit->type.state & UPPER_FLAGS) && 
            delta->value >= crit->value;
}

static status queueScaleTest(MemCh *m, i32 max){
    status r = READY;
    void *args[5];

    Queue *q = Queue_Make(m, (QueueCritFunc)Queue_timeFunc);

    i32 patCount = 4;
    i32 patIdx = 0;
    util pattern[4] = { 1, 0, 3, 2};

    q->time.delta.type.state = APPROXTIME_MILLISEC;
    q->time.delta.value = 0;

    for(i32 i = 0; i < max; i++){
        i16 time = pattern[i & 3];
        if(i >= 4){
            i32 base = i;
            base &= ~3;
            time += base;
        }
        Queue_Add(q, I32_Wrapped(m, i), ApproxTime_Make(m, APPROXTIME_MILLISEC, time)); 
    }

    Queue_Reset(q);
    i16 guard = 0;

    i32 nvalues = 0;
    i32 count = 0;

    while(q->it.p->nvalues > 0){

        if(nvalues == q->it.p->nvalues){
            count++;
            if(count == 20){
                q->type.state |= DEBUG;
            }else if(count == 21){
                args[0] = I32_Wrapped(m, q->it.p->nvalues);
                args[1] = I32_Wrapped(m, q->it.idx);
                args[2] = I32_Wrapped(m, max);
                args[3] = q;
                Out("^y.Count is $/\\@$ of $max &^0\n", args);
                break;
            }
        }
        nvalues = q->it.p->nvalues;

        if(!Guard(&guard, max * 2, FUNCNAME, FILENAME, LINENUMBER)){
            args[0] = I32_Wrapped(m, max);
            args[1] = I16_Wrapped(m, guard);
            args[2] = q;
            args[3] = &q->time.delta;
            args[4] = NULL;
            r |= Test(FALSE, "All $ Queue items were handled, too many rounds,"
                " round=$ q=@ \\@$", args);
            return r;
        }

        q->time.delta.value++;
        if(q->type.state & END){
            Queue_Reset(q);
        }

        while((Queue_Next(q) & END) == 0){
            if(Queue_Get(q) == NULL){
                r |= ERROR;
                break;
            }
            Queue_Remove(q, q->it.idx);
        }
    }

    args[0] = I32_Wrapped(m, max);
    args[1] = NULL;
    r |= Test(q->it.p->nvalues == 0, "All $ Queue items were handled", args);
    return r;
}

status QueueScale_Tests(MemCh *m){
    Debug_Push(m, NULL);
    status r = READY;
    void *args[6];

    Queue *q = Queue_Make(m, (QueueCritFunc)Queue_timeFunc);
    Queue_Next(q);

    i32 idx = 16;
    Queue_Set(q, idx, S(m, "Not Cool..."), ApproxTime_Make(m, APPROXTIME_SEC, 1));

    idx = 24;
    Queue_Set(q, idx, S(m, "Hidy Ho!"), ApproxTime_Make(m, APPROXTIME_SEC, 1));

    idx = 63;
    Queue_Set(q, idx, S(m, "Livin on the Edge!"), ApproxTime_Make(m, APPROXTIME_SEC, 1));

    idx = 64;
    Queue_Set(q, idx, S(m, "XjfoaiwuerZduio"), ApproxTime_Make(m, APPROXTIME_SEC, 1));

    idx = 65;
    Queue_Set(q, idx, S(m, "SixyFivey"), ApproxTime_Make(m, APPROXTIME_SEC, 1));

    q->time.delta.type.state = APPROXTIME_SEC;
    q->time.delta.value = 2;
    Queue_Next(q);

    args[0] = Queue_Get(q);
    args[1] = q;
    args[2] = NULL;
    r |= Test(Equals(args[0], S(m, "Not Cool...")),
        "Expected value above first slab found, have @, in @", args);

    Queue_Next(q);

    args[0] = Queue_Get(q);
    args[1] = q;
    args[2] = NULL;
    r |= Test(Equals(args[0], S(m, "Hidy Ho!")),
        "Expected value above first slab found, have @, in @", args);

    Queue_Next(q);

    args[0] = Queue_Get(q);
    args[1] = q;
    args[2] = NULL;
    r |= Test(Equals(args[0], S(m, "Livin on the Edge!")),
        "Expected value above first slab found, have @, in @", args);

    Queue_Next(q);

    args[0] = Queue_Get(q);
    args[1] = q;
    args[2] = NULL;
    r |= Test(Equals(args[0], S(m, "XjfoaiwuerZduio")),
        "Expected value above first slab found, have @, in @", args);

    Queue_Next(q);

    args[0] = Queue_Get(q);
    args[1] = q;
    args[2] = NULL;
    r |= Test(Equals(args[0],  S(m, "SixyFivey")),
        "Expected value above first slab found, have @, in @", args);

    r |= Test((queueScaleTest(m, 10) & (SUCCESS|ERROR)) == SUCCESS,
        "Max 10 scale tests finish with SUCCESS", NULL);

    r |= Test((queueScaleTest(m, 57) & (SUCCESS|ERROR)) == SUCCESS,
        "Max 57 scale tests finish with SUCCESS", NULL);

    r |= Test((queueScaleTest(m, 66) & (SUCCESS|ERROR)) == SUCCESS,
        "Max 66 scale tests finish with SUCCESS", NULL);

    r |= Test((queueScaleTest(m, 432) & (SUCCESS|ERROR)) == SUCCESS,
        "Max 432 scale tests finish with SUCCESS", NULL);

    r |= Test((queueScaleTest(m, 777) & (SUCCESS|ERROR)) == SUCCESS,
        "Max 777 scale tests finish with SUCCESS", NULL);

    r |= Test((queueScaleTest(m, 4000) & (SUCCESS|ERROR)) == SUCCESS,
        "Max 4k scale tests finish with SUCCESS", NULL);

    Return(m, r);
}
