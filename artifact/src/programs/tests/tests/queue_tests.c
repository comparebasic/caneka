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

    r |= ERROR;
    MemCh_Free(m);
    return r;
}

status QueueCriteria_Tests(MemCh *gm){
    DebugStack_Push(NULL, 0);
    status r = READY;
    Abstract *args[8];
    MemCh *m = MemCh_Make();

    Queue *q = Queue_Make(m);

    r |= ERROR;
    MemCh_Free(m);
    return r;
}

