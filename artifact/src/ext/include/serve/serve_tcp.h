Task *ServeTcp_Make(TcpCtx *ctx);
status ServeTcp_AcceptPoll(Step *st, Task *tsk);
status ServeTcp_OpenTcp(Step *st, Task *tsk);

#define TcpTask_GetPollFd(tsk) ((struct pollfd *)&(tsk)->u)
