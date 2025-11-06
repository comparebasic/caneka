#define TCP_STEP_MAX 512
#define SERV_READ_SIZE 1024
#define SERV_SEND_SIZE 1024
#define TCP_POLL_DELAY 10
#define ACCEPT_AT_ONEC_MAX 192

Task *ServeTcp_Make(TcpCtx *ctx);
