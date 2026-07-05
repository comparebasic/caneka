#include <external.h>
#include <caneka.h>
#include <test_module.h>


static status queueScaleTest(MemCh *m, i32 max){
    status r = READY;
    void *args[5];

    Queue *q = Queue_Make(m);

    QueueCrit *crit = QueueCrit_Make(m, QueueCrit_Time, ZERO);
    i32 hIdx = Queue_AddHandler(q, crit);

    i32 patCount = 4;
    i32 patIdx = 0;
    util pattern[4] = { 1, 0, 3, 2};

    ApproxTime *current = (ApproxTime *)&crit->u;
    current->type.of = TYPE_APPROXTIME;
    current->type.state = APPROXTIME_MILLISEC;
    current->value = 0;

    for(i32 i = 0; i < max; i++){
        i16 time = pattern[i & 3];
        if(i >= 4){
            i32 base = i;
            base &= ~3;
            time += base;
        }
        ApproxTime at = {
            .type = {TYPE_APPROXTIME, APPROXTIME_MILLISEC},
            .value = time 
        };
        util *ptr = (util *)&at;
        Queue_Add(q, I32_Wrapped(m, i)); 
        Queue_SetCriteria(q, hIdx, i, (util **)&ptr);
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
            args[3] = Util_Wrapped(m, crit->u);
            args[4] = NULL;
            r |= Test(FALSE, "All $ Queue items were handled, too many rounds,"
                " round=$ q=@ \\@$", args);
            return r;
        }

        ApproxTime *at = (ApproxTime *)&crit->u;
        at->value++;

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

    Queue *q = Queue_Make(m);

    QueueCrit *crit = QueueCrit_Make(m, QueueCrit_Time, ZERO);
    crit->type.state |= QUEUE_CRIT_UTIL;
    ApproxTime *current = (ApproxTime *)&crit->u;
    i32 hIdx = Queue_AddHandler(q, crit);
    crit->u = 10;
    util time = 1;

    ApproxTime oneSec = {.type = {TYPE_APPROXTIME, APPROXTIME_SEC}, .value=1};

    Queue_Next(q);

    util *ptr = (util *)&oneSec;

    i32 idx = 16;
    Queue_Set(q, idx, S(m, "Not Cool..."));
    Queue_SetCriteria(q, hIdx, idx, (util **)&ptr);

    idx = 24;
    Queue_Set(q, idx, S(m, "Hidy Ho!"));
    Queue_SetCriteria(q, hIdx, idx, (util **)&ptr);

    idx = 63;
    Queue_Set(q, idx, S(m, "Livin on the Edge!"));
    Queue_SetCriteria(q, hIdx, idx, (util **)&ptr);

    idx = 64;
    Queue_Set(q, idx, S(m, "XjfoaiwuerZduio"));
    Queue_SetCriteria(q, hIdx, idx, (util **)&ptr);

    idx = 65;
    Queue_Set(q, idx, S(m, "SixyFivey"));
    Queue_SetCriteria(q, hIdx, idx, (util **)&ptr);

    current->type.state = APPROXTIME_SEC;
    current->value = 2;
    Queue_Next(q);
    idx = 16;

    util u = 1;
    util compare = ((u << 16) | (u << 24) | (u << 63));
    args[0] = Str_Ref(m, (byte *)&compare, sizeof(util), sizeof(util), STRING_BINARY);
    args[1] = I32_Wrapped(m, 16);
    args[2] = I32_Wrapped(m, 24);
    args[3] = I32_Wrapped(m, 63);
    args[4] = Str_Ref(m, (byte *)&q->go, sizeof(util), sizeof(util), STRING_BINARY);
    args[5] = NULL;
    r |= Test(q->go == compare,
        "Expected q->go to be set to binary $ of @,@, and @, have $", args);

    if(r & ERROR){
        Return(m, r);
    }

    args[0] = Queue_Get(q);
    args[1] = q;
    args[2] = NULL;
    r |= Test(Equals(args[0], S(m, "Not Cool...")),
        "Expected value above first slab found, have @, in @", args);

    compare = (u << idx);
    args[0] = Str_Ref(m, (byte *)&q->go, sizeof(util), sizeof(util), STRING_BINARY);
    args[1] = I32_Wrapped(m, idx);
    args[2] = NULL;
    r |= Test((q->go & compare) != 0,
        "Expected q->go to be set to binary $ of 1 << @", args);

    Queue_Next(q);
    idx = 24;
    compare = (u << idx);

    args[0] = Queue_Get(q);
    args[1] = q;
    args[2] = NULL;
    r |= Test(Equals(args[0], S(m, "Hidy Ho!")),
        "Expected value above first slab found, have @, in @", args);

    args[0] = Str_Ref(m, (byte *)&q->go, sizeof(util), sizeof(util), STRING_BINARY);
    args[1] = I32_Wrapped(m, idx);
    args[2] = NULL;
    r |= Test((q->go & compare) != 0,
        "Expected q->go to be set to binary $ of 1 << @", args);

    Queue_Next(q);
    idx = 63;
    compare = (1 << idx);

    args[0] = Queue_Get(q);
    args[1] = q;
    args[2] = NULL;
    r |= Test(Equals(args[0], S(m, "Livin on the Edge!")),
        "Expected value above first slab found, have @, in @", args);

    args[0] = Str_Ref(m, (byte *)&q->go, sizeof(util), sizeof(util), STRING_BINARY);
    args[1] = I32_Wrapped(m, idx);
    args[2] = NULL;
    r |= Test((q->go & compare) != 0,
        "Expected q->go to be set to binary $ of 1 << @", args);

    Queue_Next(q);
    idx = 64;
    compare = (u << 0);

    args[0] = Queue_Get(q);
    args[1] = q;
    args[2] = NULL;
    r |= Test(Equals(args[0], S(m, "XjfoaiwuerZduio")),
        "Expected value above first slab found, have @, in @", args);

    args[0] = Str_Ref(m, (byte *)&q->go, sizeof(util), sizeof(util), STRING_BINARY);
    args[1] = I32_Wrapped(m, idx);
    args[2] = NULL;
    r |= Test((q->go & compare) != 0,
        "Expected q->go to be set to binary $ of 1 << @", args);

    Queue_Next(q);
    idx = 65;
    compare = (u << 1);

    args[0] = Queue_Get(q);
    args[1] = q;
    args[2] = NULL;
    r |= Test(Equals(args[0],  S(m, "SixyFivey")),
        "Expected value above first slab found, have @, in @", args);

    if(r & ERROR){
        Return(m, r);
    }

    args[0] = Str_Ref(m, (byte *)&q->go, sizeof(util), sizeof(util), STRING_BINARY);
    args[1] = I32_Wrapped(m, idx);
    args[2] = NULL;
    r |= Test((q->go & compare) != 0,
        "Expected q->go to be set to binary $ of 1 << @", args);

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
