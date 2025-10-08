#include <external.h>
#include <caneka.h>

status Queue_Tests(MemCh *gm){
    DebugStack_Push(NULL, 0);
    status r = READY;
    Abstract *args[5];
    MemCh *m = MemCh_Make();

    Queue *q = Queue_Make(m);

    q->type.state |= DEBUG;

    QueueCrit *crit = QueueCrit_Make(m, QueueCrit_Fds, ZERO);
    Queue_AddHandler(q, crit);

    Str *path = IoUtil_GetCwdPath(m, Str_CstrRef(m, "dist/test/queue.test"));
    i32 fd = open(Str_Cstr(m, path), O_WRONLY|O_CREAT, 00644);

    struct pollfd pfd = { fd, POLLOUT|POLLIN, 0};

    Str *one = Str_CstrRef(m, "ItemOne");
    Queue_Set(q, 3, (Abstract *)one);

    Queue_SetCriteria(q, 0, 3, (util *)&pfd);

    status re = Queue_Next(q);
    i32 idx = Queue_GetIdx(q);
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
    
    /*
    Str *s = Str_CstrRef(m, "Some action here");
    write(fd, s, s->length);
    close(fd);

    re = Queue_Next(q);
    r |= Test((re & (SUCCESS|ERROR|END)) == SUCCESS,
        "Queue Next has value success with no file descriptor action.", NULL);

    item = Queue_Get(q);
    r |= Test(item == NULL,
        "Queue item matches expected after write.", NULL);

    r |= Test(Queue_GetIdx(q) == 3,
        "Queue idx is expected", NULL);
    */

    MemCh_Free(m);
    return r;
}
