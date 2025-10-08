#define TcpTask_GetPollFd(tsk) ((struct pollfd *)&(tsk)->u)
status TcpTask_ReadToRbl(Step *st, Task *tsk);
status TcpTask_WriteFromOut(Step *st, Task *tsk);
status TcpTask_ExpectRecv(Step *st, Task *tsk);
status TcpTask_ExpectSend(Step *st, Task *tsk);
