#include <external.h>
#include <caneka.h>
#include <test_module.h>


status Queue_Tests(MemCh *m){
    Debug_Push(m, NULL);
    status r = READY;
    void *args[5];

    Queue *q = Queue_Make(m);
    i32 idx = 0;

    QueueCrit *crit = QueueCrit_Make(m, QueueCrit_Fds, ZERO);
    Queue_AddHandler(q, crit);

    Str *path = IoUtil_GetCwdPath(m, Str_CstrRef(m, "dist/test/queue.test"));
    i32 fd = open(Str_Cstr(m, path), O_WRONLY|O_CREAT, 00644);

    struct pollfd pfd = { fd, POLLOUT|POLLIN, 0};

    Str *one = Str_CstrRef(m, "ItemOne");
    Queue_Set(q, 3, one);

    util *ptr = &pfd;
    Queue_SetCriteria(q, 0, 3, (util **)&ptr);

    status re = Queue_Next(q);
    idx = Queue_GetIdx(q);
    args[0] = I32_Wrapped(m, idx);
    args[1] = q;
    args[2] = NULL;
    r |= Test(idx == 3,
        "Queue idx is on added item, expected 3, have $ for @", args);

    void *item = Queue_Get(q);
    args[0] = one;
    args[1] = item;
    args[2] = NULL;
    r |= Test((Str *)item == one,
        "Queue item is first item, expected &, have &", args);

    Return(m, r);
}

status QueueAddRemove_Tests(MemCh *m){
    Debug_Push(m, NULL);
    status r = READY;
    void *args[8];

    Queue *q = Queue_Make(m);
    i32 idx = Queue_Add(q, Str_FromCstr(m, "Temporary", ZERO));
    Queue_Remove(q, idx);

    r |= Test((q->availableIt.metrics.selected == 0),
        "Queue has the 0th item available", args);

    r |= Test(q->it.p->nvalues == 0, "No items after first item is removed", NULL);

    Str *s = Str_FromCstr(m, "Alpha", ZERO);
    idx = Queue_Add(q, s);

    r |= Test(q->it.p->nvalues == 1 && q->it.p->max_idx == 0, "first item is in the first slot", NULL);
    args[0] = Span_Get(q->it.p, 0);
    args[1] = Str_FromCstr(m, "Alpha", ZERO);
    args[2] = NULL;
    r |= Test(Equals(args[0], args[1]), "first item is in the first slot, expected @, have @", args);

    s = Str_FromCstr(m, "Bravo", ZERO);
    idx = Queue_Add(q, s);

    s = Str_FromCstr(m, "Charlie", ZERO);
    idx = Queue_Add(q, s);

    args[0] = Span_Get(q->it.p, 0);
    args[1] = Str_FromCstr(m, "Alpha", ZERO);
    args[2] = Span_Get(q->it.p, 1);
    args[3] = Str_FromCstr(m, "Bravo", ZERO);
    args[4] = Span_Get(q->it.p, 2);
    args[5] = Str_FromCstr(m, "Charlie", ZERO);
    args[6] = NULL;
    r |= Test(Equals(args[0], args[1]) && Equals(args[2], args[3]) && Equals(args[4], args[5]), "Expected @ = @, @ = @, @ = @", args);


    Queue_Remove(q, 1);

    args[0] = Span_Get(q->it.p, 0);
    args[1] = Str_FromCstr(m, "Alpha", ZERO);
    args[2] = Span_Get(q->it.p, 1);
    args[3] = NULL;
    args[4] = Span_Get(q->it.p, 2);
    args[5] = Str_FromCstr(m, "Charlie", ZERO);
    args[6] = NULL;

    r |= Test(Equals(args[0], args[1]) && args[2] == NULL && args[3] == NULL && Equals(args[4], args[5]), "Expected @ = @, null(@) = null(@), @ = @", args);

    s = Str_FromCstr(m, "Bravo-two", ZERO);
    idx = Queue_Add(q, s);

    args[0] = Span_Get(q->it.p, 0);
    args[1] = Str_FromCstr(m, "Alpha", ZERO);
    args[2] = Span_Get(q->it.p, 1);
    args[3] = Str_FromCstr(m, "Bravo-two", ZERO);
    args[4] = Span_Get(q->it.p, 2);
    args[5] = Str_FromCstr(m, "Charlie", ZERO);
    args[6] = NULL;

    r |= Test(Equals(args[0], args[1]) && Equals(args[2], args[3]) && Equals(args[4], args[5]), "Expected @ = @, @ = @, @ = @", args);

    Return(m, r);
}

status QueueIter_Tests(MemCh *m){
    Debug_Push(m, NULL);
    status r = READY;
    void *args[8];

    Queue *q = Queue_Make(m);

    r |= Test(Queue_Next(q) & END, "Empty Queue_Next returns END flag", NULL);
    
    Str *s = Str_FromCstr(m, "One", ZERO);
    Queue_Add(q, s);

    void *expected[10];
    expected[0] = Str_FromCstr(m, "One", ZERO);
    expected[1] = NULL;
    i32 i = 0;
    while((Queue_Next(q) & END) == 0){
        args[0] = expected[i];
        args[1] = Queue_Get(q);
        args[2] = NULL;
        r |= Test(Equals(args[1], expected[i]),
            "Queue item matches, expected @, have @", args);
        i++;
    };

    args[0] = I32_Wrapped(m, i);
    args[1] = q;
    args[2] = NULL;
    r |= Test(i == 1, "Queue runs for the number of items in it, have $, for @", args);
    
    s = Str_FromCstr(m, "Two", ZERO);
    Queue_Add(q, s);
    s = Str_FromCstr(m, "Three", ZERO);
    Queue_Add(q, s);
    s = Str_FromCstr(m, "Four", ZERO);
    Queue_Add(q, s);
    s = Str_FromCstr(m, "Five", ZERO);
    Queue_Add(q, s);

    expected[0] = Str_FromCstr(m, "One", ZERO);
    expected[1] = Str_FromCstr(m, "Two", ZERO);
    expected[2] = Str_FromCstr(m, "Three", ZERO);
    expected[3] = Str_FromCstr(m, "Four", ZERO);
    expected[4] = Str_FromCstr(m, "Five", ZERO);
    expected[5] = NULL;
    i = 0;
    while((Queue_Next(q) & END) == 0){
        args[0] = expected[i];
        args[1] = Queue_Get(q);
        args[2] = NULL;
        r |= Test(Equals(args[1], expected[i]),
            "Queue item matches, expected @, have @", args);
        i++;
    };

    args[0] = I32_Wrapped(m, i);
    args[1] = q;
    args[2] = NULL;
    r |= Test(i == 5, "Queue runs for the number of items in it, have $, for @", args);

    Queue_Remove(q, 3);

    expected[0] = Str_FromCstr(m, "One", ZERO);
    expected[1] = Str_FromCstr(m, "Two", ZERO);
    expected[2] = Str_FromCstr(m, "Three", ZERO);
    expected[3] = NULL;
    expected[4] = Str_FromCstr(m, "Five", ZERO);
    expected[5] = NULL;
    i = 0;
    while((Queue_Next(q) & END) == 0){
        args[0] = expected[i];
        args[1] = Queue_Get(q);
        args[2] = NULL;
        r |= Test(args[0] == NULL && args[1] == NULL || Equals(args[1], expected[i]),
            "Queue item matches, expected @, have @", args);
        i++;
    };

    s = Str_FromCstr(m, "Four-two", ZERO);
    Queue_Add(q, s);

    expected[0] = Str_FromCstr(m, "One", ZERO);
    expected[1] = Str_FromCstr(m, "Two", ZERO);
    expected[2] = Str_FromCstr(m, "Three", ZERO);
    expected[3] = Str_FromCstr(m, "Four-two", ZERO);
    expected[4] = Str_FromCstr(m, "Five", ZERO);
    expected[5] = NULL;
    i = 0;
    while((Queue_Next(q) & END) == 0){
        args[0] = expected[i];
        args[1] = Queue_Get(q);
        args[2] = NULL;
        r |= Test(args[0] == NULL && args[1] == NULL || Equals(args[1], expected[i]),
            "Queue item matches, expected @, have @", args);
        i++;
    };

    args[0] = I32_Wrapped(m, i);
    args[1] = q;
    args[2] = NULL;
    r |= Test(i == 5, "Queue runs for the number of items in it, have $, for @", args);

    Return(m, r);
}

status QueueCriteria_Tests(MemCh *m){
    Debug_Push(m, NULL);
    status r = READY;
    void *args[8];
    i32 i = 0;

    Queue *q = Queue_Make(m);

    QueueCrit *crit = QueueCrit_Make(m, QueueCrit_Time, ZERO);
    i32 hIdx = Queue_AddHandler(q, crit);
    QueueCrit *critFds = QueueCrit_Make(m, QueueCrit_Fds, ZERO);
    i32 fdIdx = Queue_AddHandler(q, critFds);

    ApproxTime *current = (ApproxTime *)&crit->u;
    current->type.of = TYPE_APPROXTIME;
    current->type.state = APPROXTIME_SEC;
    current->value = 0;

    Str *s = Str_FromCstr(m, "Two Days", ZERO);
    
    ApproxTime compare = {.type = {TYPE_APPROXTIME, APPROXTIME_DAY}, .value =2};
    i32 idx = Queue_Add(q, s);
    util *ptr = (util *)&compare;
    Queue_SetCriteria(q, hIdx, idx, (util **)&ptr);

    s = Str_FromCstr(m, "Three Seconds", ZERO);
    idx = Queue_Add(q, s);
    compare.type.state = APPROXTIME_SEC;
    compare.value = 3;
    Queue_SetCriteria(q, hIdx, idx, (util **)&ptr);

    s = Str_FromCstr(m, "Ten Minutes", ZERO);
    compare.type.state = APPROXTIME_MIN;
    compare.value = 10;
    idx = Queue_Add(q, s);
    Queue_SetCriteria(q, hIdx, idx, (util **)&ptr);

    status re = Queue_Next(q);
    args[0] = Type_StateVec(m, q->type.of, q->type.state);
    args[1] = NULL;
    r |= Test(q->type.state & END, "No items will run at the present time, flags @ ", args);

    current = (ApproxTime *)&crit->u;
    current->type.state = APPROXTIME_SEC;
    current->value = 4;

    void *expected[10];
    expected[0] = Str_FromCstr(m, "Three Seconds", ZERO);
    expected[1] = NULL;
    i = 0;

    while((Queue_Next(q) & END) == 0){
        args[0] = expected[i];
        args[1] = Queue_Get(q);
        args[2] = NULL;
        r |= Test(args[0] == NULL && args[1] == NULL || Equals(args[1], expected[i]),
            "Queue item matches, expected @, have @", args);
        i++;
    };

    args[0] = I32_Wrapped(m, i);
    args[1] = (ApproxTime *)&crit->u;
    args[2] = NULL;
    r |= Test(i == 1, "Only one item was available to run, i is $, at $ from start", args);

    current->type.state = APPROXTIME_MIN;
    current->value = 15;

    expected[0] = Str_FromCstr(m, "Three Seconds", ZERO);
    expected[1] = Str_FromCstr(m, "Ten Minutes", ZERO);
    expected[2] = NULL;
    i = 0;

    while((Queue_Next(q) & END) == 0){
        args[0] = expected[i];
        args[1] = Queue_Get(q);
        args[2] = NULL;
        r |= Test(args[0] == NULL && args[1] == NULL || Equals(args[1], expected[i]),
            "Queue item matches, expected @, have @", args);
        i++;
    };

    args[0] = I32_Wrapped(m, i);
    args[1] = current;
    args[2] = NULL;
    r |= Test(i == 2, "Only one item was available to run, i is $, at @ from start", args);


    current->type.state = APPROXTIME_DAY;
    current->value = 3;

    expected[0] = Str_FromCstr(m, "Two Days", ZERO);
    expected[1] = Str_FromCstr(m, "Three Seconds", ZERO);
    expected[2] = Str_FromCstr(m, "Ten Minutes", ZERO);
    expected[3] = NULL;
    i = 0;

    while((Queue_Next(q) & END) == 0){
        args[0] = expected[i];
        args[1] = Queue_Get(q);
        args[2] = NULL;
        r |= Test(args[0] == NULL && args[1] == NULL || Equals(args[1], expected[i]),
            "Queue item matches, expected @, have @", args);
        i++;
    };

    args[0] = I32_Wrapped(m, i);
    args[1] = current;
    args[2] = NULL;
    r |= Test(i == 2, "Only one item was available to run, i is $, at $ days from start", args);

    Queue_Remove(q, 0);

    s = Str_FromCstr(m, "File Descriptor", ZERO);

    Buff *bf = Buff_Make(m, ZERO);
    Str *path = IoUtil_GetCwdPath(m, Str_CstrRef(m, "dist/test/queue.test"));
    File_Open(bf, path, O_RDONLY);

    idx = Queue_Add(q, s);
    struct pollfd pfd = { bf->fd, POLLOUT, 0};
    util *ptr = (util *)&pfd;
    Queue_SetCriteria(q, fdIdx, idx, (util **)&ptr);

    Queue_Reset(q);

    current->type.state = APPROXTIME_SEC;
    current->value = 10;

    expected[0] = Str_FromCstr(m, "File Descriptor", ZERO);
    expected[1] = Str_FromCstr(m, "Three Seconds", ZERO);
    expected[2] = NULL;
    i = 0;

    while((Queue_Next(q) & END) == 0){
        args[0] = expected[i];
        args[1] = Queue_Get(q);
        args[2] = NULL;
        r |= Test(args[0] == NULL && args[1] == NULL || Equals(args[1], expected[i]),
            "Queue item matches, expected @, have @", args);
        i++;
    };

    args[0] = I32_Wrapped(m, i);
    args[1] = current;
    args[2] = NULL;
    r |= Test(i == 2, "PollFd set to Read: two item was available to run, i is $, at $ seconds from start", args);

    struct pollfd pfd2 = { bf->fd, ZERO, 0};
    util *ptr2 = &pfd2;
    Queue_SetCriteria(q, fdIdx, 0, (util **)&ptr2);

    expected[0] = Str_FromCstr(m, "Three Seconds", ZERO);
    expected[1] = NULL;
    i = 0;

    while((Queue_Next(q) & END) == 0){
        args[0] = expected[i];
        args[1] = Queue_Get(q);
        args[2] = NULL;
        r |= Test(args[0] == NULL && args[1] == NULL || Equals(args[1], expected[i]),
            "Queue item matches, expected @, have @", args);
        i++;
    };

    args[0] = I32_Wrapped(m, i);
    args[1] = current;
    args[2] = NULL;
    r |= Test(i == 1, "No direction set on pollfd: Only one item was available to run, i is $, at $ seconds from start", args);

    File_Close(bf);

    Return(m, r);
}

