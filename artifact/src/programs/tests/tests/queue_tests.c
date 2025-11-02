#include <external.h>
#include <caneka.h>

status Queue_Tests(MemCh *gm){
    DebugStack_Push(NULL, 0);
    status r = READY;
    Abstract *args[5];
    MemCh *m = MemCh_Make();

    Queue *q = Queue_Make(m);
    i32 idx = 0;

    QueueCrit *crit = QueueCrit_Make(m, QueueCrit_Fds, ZERO);
    Queue_AddHandler(q, crit);

    Str *path = IoUtil_GetCwdPath(m, Str_CstrRef(m, "dist/test/queue.test"));
    i32 fd = open(Str_Cstr(m, path), O_WRONLY|O_CREAT, 00644);

    struct pollfd pfd = { fd, POLLOUT|POLLIN, 0};

    Str *one = Str_CstrRef(m, "ItemOne");
    Queue_Set(q, 3, (Abstract *)one);

    Queue_SetCriteria(q, 0, 3, (util *)&pfd);

    status re = Queue_Next(q);
    idx = Queue_GetIdx(q);
    args[0] = (Abstract *)I32_Wrapped(m, idx);
    args[1] = NULL;
    r |= Test(idx == 3,
        "Queue idx is on added item, expected 3, have $", args);

    Abstract *item = Queue_Get(q);
    args[0] = (Abstract *)one;
    args[1] = item;
    args[2] = NULL;
    r |= Test((Str *)item == one,
        "Queue item is first item, expected &, have &", args);

    MemCh_Free(m);
    return r;
}

status QueueAddRemove_Tests(MemCh *gm){
    DebugStack_Push(NULL, 0);
    status r = READY;
    Abstract *args[8];
    MemCh *m = MemCh_Make();

    Queue *q = Queue_Make(m);
    i32 idx = Queue_Add(q, (Abstract *)Str_FromCstr(m, "Temporary", ZERO));
    Queue_Remove(q, idx);

    r |= Test((q->availableIt.metrics.selected == 0),
        "Queue has the 0th item available", args);

    r |= Test(q->it.p->nvalues == 0, "No items after first item is removed", NULL);

    Str *s = Str_FromCstr(m, "Alpha", ZERO);
    idx = Queue_Add(q, (Abstract *)s);

    r |= Test(q->it.p->nvalues == 1 && q->it.p->max_idx == 0, "first item is in the first slot", NULL);
    args[0] = (Abstract *)Span_Get(q->it.p, 0);
    args[1] = (Abstract *)Str_FromCstr(m, "Alpha", ZERO);
    args[2] = NULL;
    r |= Test(Equals(args[0], args[1]), "first item is in the first slot, expected @, have @", args);

    s = Str_FromCstr(m, "Bravo", ZERO);
    idx = Queue_Add(q, (Abstract *)s);

    s = Str_FromCstr(m, "Charlie", ZERO);
    idx = Queue_Add(q, (Abstract *)s);

    args[0] = (Abstract *)Span_Get(q->it.p, 0);
    args[1] = (Abstract *)Str_FromCstr(m, "Alpha", ZERO);
    args[2] = (Abstract *)Span_Get(q->it.p, 1);
    args[3] = (Abstract *)Str_FromCstr(m, "Bravo", ZERO);
    args[4] = (Abstract *)Span_Get(q->it.p, 2);
    args[5] = (Abstract *)Str_FromCstr(m, "Charlie", ZERO);
    args[6] = NULL;
    r |= Test(Equals(args[0], args[1]) && Equals(args[2], args[3]) && Equals(args[4], args[5]), "Expected @ = @, @ = @, @ = @", args);


    Queue_Remove(q, 1);

    args[0] = (Abstract *)Span_Get(q->it.p, 0);
    args[1] = (Abstract *)Str_FromCstr(m, "Alpha", ZERO);
    args[2] = (Abstract *)Span_Get(q->it.p, 1);
    args[3] = (Abstract *)NULL;
    args[4] = (Abstract *)Span_Get(q->it.p, 2);
    args[5] = (Abstract *)Str_FromCstr(m, "Charlie", ZERO);
    args[6] = NULL;

    r |= Test(Equals(args[0], args[1]) && args[2] == NULL && args[3] == NULL && Equals(args[4], args[5]), "Expected @ = @, null(@) = null(@), @ = @", args);

    s = Str_FromCstr(m, "Bravo-two", ZERO);
    idx = Queue_Add(q, (Abstract *)s);

    args[0] = (Abstract *)Span_Get(q->it.p, 0);
    args[1] = (Abstract *)Str_FromCstr(m, "Alpha", ZERO);
    args[2] = (Abstract *)Span_Get(q->it.p, 1);
    args[3] = (Abstract *)Str_FromCstr(m, "Bravo-two", ZERO);
    args[4] = (Abstract *)Span_Get(q->it.p, 2);
    args[5] = (Abstract *)Str_FromCstr(m, "Charlie", ZERO);
    args[6] = NULL;

    r |= Test(Equals(args[0], args[1]) && Equals(args[2], args[3]) && Equals(args[4], args[5]), "Expected @ = @, @ = @, @ = @", args);

    MemCh_Free(m);
    return r;
}

status QueueIter_Tests(MemCh *gm){
    DebugStack_Push(NULL, 0);
    status r = READY;
    Abstract *args[8];
    MemCh *m = MemCh_Make();

    Queue *q = Queue_Make(m);

    r |= Test(Queue_Next(q) & END, "Empty Queue_Next returns END flag", NULL);
    
    Str *s = Str_FromCstr(m, "One", ZERO);
    Queue_Add(q, (Abstract *)s);

    Abstract *expected[10];
    expected[0] = (Abstract *)Str_FromCstr(m, "One", ZERO);
    expected[1] = NULL;
    i32 i = 0;
    while((Queue_Next(q) & END) == 0){
        args[0] = expected[i];
        args[1] = Queue_Get(q);
        args[2] = NULL;
        r |= Test(Equals((Abstract *)args[1], (Abstract *)expected[i]),
            "Queue item matches, expected @, have @", args);
        i++;
    };

    args[0] = (Abstract *)I32_Wrapped(m, i);
    args[1] = (Abstract *)q;
    args[2] = NULL;
    r |= Test(i == 1, "Queue runs for the number of items in it, have $, for @", args);
    
    s = Str_FromCstr(m, "Two", ZERO);
    Queue_Add(q, (Abstract *)s);
    s = Str_FromCstr(m, "Three", ZERO);
    Queue_Add(q, (Abstract *)s);
    s = Str_FromCstr(m, "Four", ZERO);
    Queue_Add(q, (Abstract *)s);
    s = Str_FromCstr(m, "Five", ZERO);
    Queue_Add(q, (Abstract *)s);

    expected[0] = (Abstract *)Str_FromCstr(m, "One", ZERO);
    expected[1] = (Abstract *)Str_FromCstr(m, "Two", ZERO);
    expected[2] = (Abstract *)Str_FromCstr(m, "Three", ZERO);
    expected[3] = (Abstract *)Str_FromCstr(m, "Four", ZERO);
    expected[4] = (Abstract *)Str_FromCstr(m, "Five", ZERO);
    expected[5] = NULL;
    i = 0;
    while((Queue_Next(q) & END) == 0){
        args[0] = expected[i];
        args[1] = Queue_Get(q);
        args[2] = NULL;
        r |= Test(Equals((Abstract *)args[1], (Abstract *)expected[i]),
            "Queue item matches, expected @, have @", args);
        i++;
    };

    args[0] = (Abstract *)I32_Wrapped(m, i);
    args[1] = (Abstract *)q;
    args[2] = NULL;
    r |= Test(i == 5, "Queue runs for the number of items in it, have $, for @", args);

    Queue_Remove(q, 3);

    expected[0] = (Abstract *)Str_FromCstr(m, "One", ZERO);
    expected[1] = (Abstract *)Str_FromCstr(m, "Two", ZERO);
    expected[2] = (Abstract *)Str_FromCstr(m, "Three", ZERO);
    expected[3] = NULL;
    expected[4] = (Abstract *)Str_FromCstr(m, "Five", ZERO);
    expected[5] = NULL;
    i = 0;
    while((Queue_Next(q) & END) == 0){
        args[0] = expected[i];
        args[1] = Queue_Get(q);
        args[2] = NULL;
        r |= Test(args[0] == NULL && args[1] == NULL || Equals((Abstract *)args[1], (Abstract *)expected[i]),
            "Queue item matches, expected @, have @", args);
        i++;
    };

    s = Str_FromCstr(m, "Four-two", ZERO);
    Queue_Add(q, (Abstract *)s);

    expected[0] = (Abstract *)Str_FromCstr(m, "One", ZERO);
    expected[1] = (Abstract *)Str_FromCstr(m, "Two", ZERO);
    expected[2] = (Abstract *)Str_FromCstr(m, "Three", ZERO);
    expected[3] = (Abstract *)Str_FromCstr(m, "Four-two", ZERO);
    expected[4] = (Abstract *)Str_FromCstr(m, "Five", ZERO);
    expected[5] = NULL;
    i = 0;
    while((Queue_Next(q) & END) == 0){
        args[0] = expected[i];
        args[1] = Queue_Get(q);
        args[2] = NULL;
        r |= Test(args[0] == NULL && args[1] == NULL || Equals((Abstract *)args[1], (Abstract *)expected[i]),
            "Queue item matches, expected @, have @", args);
        i++;
    };

    args[0] = (Abstract *)I32_Wrapped(m, i);
    args[1] = (Abstract *)q;
    args[2] = NULL;
    r |= Test(i == 5, "Queue runs for the number of items in it, have $, for @", args);

    MemCh_Free(m);
    return r;
}

status QueueCriteria_Tests(MemCh *gm){
    DebugStack_Push(NULL, 0);
    status r = READY;
    Abstract *args[8];
    MemCh *m = MemCh_Make();
    i32 i = 0;

    Queue *q = Queue_Make(m);

    QueueCrit *crit = QueueCrit_Make(m, QueueCrit_Time, ZERO);
    i32 hIdx = Queue_AddHandler(q, crit);

    microTime start = MicroTime_Now();
    crit->u = (util)start;

    Str *s = Str_FromCstr(m, "Two Days", ZERO);
    microTime time = start + (TIME_DAY*2);
    i32 idx = Queue_Add(q, (Abstract *)s);
    Queue_SetCriteria(q, hIdx, idx, (util *)&time);

    s = Str_FromCstr(m, "Three Seconds", ZERO);
    time = start + (TIME_SEC*3);
    idx = Queue_Add(q, (Abstract *)s);
    Queue_SetCriteria(q, hIdx, idx, (util *)&time);

    s = Str_FromCstr(m, "Ten Minutes", ZERO);
    time = start + (TIME_MIN*10);
    idx = Queue_Add(q, (Abstract *)s);
    Queue_SetCriteria(q, hIdx, idx, (util *)&time);

    status re = Queue_Next(q);
    args[0] = (Abstract *)Type_StateVec(m, q->type.of, q->type.state);
    args[1] = NULL;
    r |= Test(q->type.state & END, "No items will run at the presetn time, flags @ ", args);

    crit->u = (util)start+(TIME_SEC*4);

    Abstract *expected[10];
    expected[0] = (Abstract *)Str_FromCstr(m, "Three Seconds", ZERO);
    expected[1] = NULL;
    i = 0;

    while((Queue_Next(q) & END) == 0){
        args[0] = expected[i];
        args[1] = Queue_Get(q);
        args[2] = NULL;
        r |= Test(args[0] == NULL && args[1] == NULL || Equals((Abstract *)args[1], (Abstract *)expected[i]),
            "Queue item matches, expected @, have @", args);
        i++;
    };

    args[0] = (Abstract *)I32_Wrapped(m, i);
    args[1] = (Abstract *)I64_Wrapped(m, ((microTime)crit->u - start) / TIME_SEC);
    args[2] = NULL;
    r |= Test(i == 1, "Only one item was available to run, i is $, at $ seconds from start", args);

    r |= ERROR;
    MemCh_Free(m);
    return r;
}

