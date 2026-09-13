#include <external.h>
#include <caneka.h>
#include <test_module.h>


static boolean Queue_timeFunc(Queue *q, Str *item, ApproxTime *crit){
    ApproxTime *delta = &q->time.delta;
    return (delta->type.state & UPPER_FLAGS) &&     
            (delta->type.state & UPPER_FLAGS) >= (crit->type.state & UPPER_FLAGS) && 
            delta->value >= crit->value;
}

status QueueAddRemove_Tests(MemCh *m){
    Debug_Push(m, NULL);
    status r = READY;
    void *args[8];

    Queue *q = Queue_Make(m, (QueueCritFunc)Queue_timeFunc);
    i32 idx = Queue_Add(q, Str_FromCstr(m, "Temporary", ZERO),
        ApproxTime_Make(m, APPROXTIME_MILLISEC, 0));
    Queue_Remove(q, idx);

    r |= Test((q->availableIt.metrics.selected == 0),
        "Queue has the 0th item available", args);

    r |= Test(q->it.p->nvalues == 0, "No items after first item is removed", NULL);


    Str *s = Str_FromCstr(m, "Alpha", ZERO);
    idx = Queue_Add(q, s, ApproxTime_Make(m, APPROXTIME_MILLISEC, 0));

    r |= Test(q->it.p->nvalues == 1 && q->it.p->max_idx == 0, "first item is in the first slot", NULL);
    args[0] = Span_Get(q->it.p, 0);
    args[1] = Str_FromCstr(m, "Alpha", ZERO);
    args[2] = NULL;
    r |= Test(Equals(args[0], args[1]), "first item is in the first slot, expected @, have @", args);

    s = Str_FromCstr(m, "Bravo", ZERO);
    idx = Queue_Add(q, s, ApproxTime_Make(m, APPROXTIME_MILLISEC, 0));

    s = Str_FromCstr(m, "Charlie", ZERO);
    idx = Queue_Add(q, s, ApproxTime_Make(m, APPROXTIME_MILLISEC, 0));

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
    idx = Queue_Add(q, s, ApproxTime_Make(m, APPROXTIME_MILLISEC, 0));

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

    Queue *q = Queue_Make(m, (QueueCritFunc)Queue_timeFunc);

    r |= Test(Queue_Next(q) & END, "Empty Queue_Next returns END flag", NULL);
    
    Str *s = Str_FromCstr(m, "One", ZERO);
    Queue_Add(q, s, ApproxTime_Make(m, APPROXTIME_MILLISEC, 0));

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
    Queue_Add(q, s, ApproxTime_Make(m, APPROXTIME_MILLISEC, 10));
    s = Str_FromCstr(m, "Three", ZERO);
    Queue_Add(q, s, ApproxTime_Make(m, APPROXTIME_MILLISEC, 20));
    s = Str_FromCstr(m, "Four", ZERO);
    Queue_Add(q, s, ApproxTime_Make(m, APPROXTIME_MILLISEC, 30));
    s = Str_FromCstr(m, "Five", ZERO);
    Queue_Add(q, s, ApproxTime_Make(m, APPROXTIME_MILLISEC, 40));

    expected[0] = Str_FromCstr(m, "One", ZERO);
    expected[1] = Str_FromCstr(m, "Two", ZERO);
    expected[2] = Str_FromCstr(m, "Three", ZERO);
    expected[3] = Str_FromCstr(m, "Four", ZERO);
    expected[5] = NULL;

    q->time.delta.value = 35;

    i = 0;
    Queue_Reset(q);
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
    r |= Test(i == 4, "Queue runs for the number of items in it, have $, for @", args);

    q->time.delta.value = 40;

    expected[0] = Str_FromCstr(m, "One", ZERO);
    expected[1] = Str_FromCstr(m, "Two", ZERO);
    expected[2] = Str_FromCstr(m, "Three", ZERO);
    expected[3] = Str_FromCstr(m, "Four", ZERO);
    expected[4] = Str_FromCstr(m, "Five", ZERO);
    expected[5] = NULL;

    q->time.delta.value = 35;

    i = 0;
    Queue_Reset(q);
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
    r |= Test(i == 4, "Queue runs for the number of items in it, have $, for @", args);


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
    Queue_Add(q, s, ApproxTime_Make(m, APPROXTIME_MILLISEC, 0));

    expected[0] = Str_FromCstr(m, "One", ZERO);
    expected[1] = Str_FromCstr(m, "Two", ZERO);
    expected[2] = Str_FromCstr(m, "Three", ZERO);
    expected[3] = Str_FromCstr(m, "Four-two", ZERO);
    expected[4] = Str_FromCstr(m, "Five", ZERO);
    expected[5] = NULL;

    q->time.delta.value = 41;

    i = 0;
    Queue_Reset(q);
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

    Queue *q = Queue_Make(m, (QueueCritFunc)Queue_timeFunc);

    Str *s = Str_FromCstr(m, "Two Days", ZERO);
    i32 idx = Queue_Add(q, s, ApproxTime_Make(m, APPROXTIME_DAY, 2));
    s = Str_FromCstr(m, "Three Seconds", ZERO);
    idx = Queue_Add(q, s, ApproxTime_Make(m, APPROXTIME_SEC, 3));
    s = Str_FromCstr(m, "Ten Minutes", ZERO);
    idx = Queue_Add(q, s, ApproxTime_Make(m, APPROXTIME_MIN, 10));

    status re = Queue_Next(q);
    args[0] = Type_StateVec(m, q->type.of, q->type.state);
    args[1] = NULL;
    r |= Test(q->type.state & END, "No items will run at the present time, flags @ ", args);

    q->time.delta.type.state = APPROXTIME_SEC;
    q->time.delta.value = 4;

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
    args[1] = &q->time.delta;
    args[2] = NULL;
    r |= Test(i == 1, "Only one item was available to run, i is $, at $ from start", args);

    q->time.delta.type.state = APPROXTIME_MIN;
    q->time.delta.value = 15;

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
    args[1] = &q->time.delta;
    args[2] = NULL;
    r |= Test(i == 2, "Only one item was available to run, i is $, at @ from start", args);


    q->time.delta.type.state = APPROXTIME_DAY;
    q->time.delta.value = 3;

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
    args[1] = &q->time.delta;
    args[2] = NULL;
    r |= Test(i == 2, "Two items are available to run, i is $, at $ days from start", args);

    q->time.delta.type.state = APPROXTIME_DAY;
    q->time.delta.value = 3;

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
    args[1] = &q->time.delta;
    args[2] = NULL;
    r |= Test(i == 2, "All three items are available to run, i is $, at $ days from start", args);

    Return(m, r);
}

