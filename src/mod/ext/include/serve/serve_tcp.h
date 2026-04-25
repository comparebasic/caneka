#define TCP_LISTEN_BACKLOG 192
#define TCP_ZERO_REQ_DELAY 5000
struct pollfd *Server_TcpGetPollFd(Req *req);
void Server_ServeTcp(Server *srv);
