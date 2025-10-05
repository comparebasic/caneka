#include <external.h>
#include <caneka.h>

status Queue_Tests(MemCh *gm){
    DebugStack_Push(NULL, 0);
    status r = READY;
    MemCh *m = MemCh_Make();

    Queue *q = Queue_Make(m);
    QueueCrit *crit = QueueCrit_Make(m, QueueCrit_Fds, ZERO);
    Queue_AddHandler(q, crit);

    Str *path = IoUtil_GetCwdPath(m, Str_CstrRef(m, "dist/test/queue.test"));
    i32 fd = open(Str_Cstr(m, path), O_WRONLY|O_CREAT, 00644);

    struct pollfd pfd = { fd, POLLIN, POLLNAL};

    Str *one = Str_CstrRef(m, "ItemOne");
    Queue_Set(q, 3, (Abstract *)one);

    Queue_SetCriteria(q, 0, 3, (util *)&pfd);

    status re = Queue_Next(q);
    r |= Test((re & (SUCCESS|ERROR|END)) == END,
        "Queue Next has end with no file descriptor action", NULL);

    Abstract *item = Queue_Get(q);
    r |= Test(item == NULL,
        "Queue item is NULL with no file descriptor action", NULL);
    
    Str *s = Str_CstrRef(m, "Some action here");
    write(fd, s, s->length);
    close(fd);

    status re = Queue_Next(q);
    r |= Test((re & (SUCCESS|ERROR|END)) == SUCCESS,
        "Queue Next has value success with no file descriptor action.", NULL);

    Abstract *item = Queue_Get(q);
    r |= Test(item == NULL,
        "Queue item matches expected after write.", NULL);

    r |= Test(Queue_GetIdx(q) == 3,
        "Queue idx is expected", NULL);

    MemCh_Free(m);
    return r;
}
