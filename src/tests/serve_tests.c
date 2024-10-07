#include <external.h>
#include <caneka.h>
#include <testsuite.h>
#include <example.h>

static char *sid = "Azjfhuei3";
static char *reqStart_cstr = "GET /Azjfhuei3 HTTP/1.1\r\nContent-Leng";  
static char *reqEnd_cstr = "th: 9\r\nHost: test.example.com\r\n\r\n{\"id\":23}";  

static int testConnect(){
	struct sockaddr_in server;
	int incoming_sc = socket(AF_INET , SOCK_STREAM , 0);
	server.sin_addr.s_addr = inet_addr("0.0.0.0");
	server.sin_family = AF_INET;
	server.sin_port = htons( TEST_PORT );

    int r = connect(incoming_sc , (struct sockaddr *)&server , sizeof(server));
    if(r == 0){
        fcntl(incoming_sc, F_SETFL, O_NONBLOCK);
        return incoming_sc;
    }else{
        printf("Error connecting to test socket %s\n", strerror(errno));
        close(incoming_sc);
        return -1;
    }
}

status Serve_Tests(MemCtx *gm){
    status r = TEST_OK;
    MemCtx *m = MemCtx_Make();

    ProtoDef *def = HttpProtoDef_Make(m);
    def->getHandler = Example_getHandler;
    Serve *sctx = Serve_Make(m, def);
    
    r |=  Serve_PreRun(sctx, TEST_PORT);

    int sock = testConnect();
    Serve_AcceptRound(sctx);
    r |= Test(sock >= 0, "Expect socket to connect has fd %d", sock);
    send(sock, reqStart_cstr, strlen(reqStart_cstr), 0);
    Serve_ServeRound(sctx);
    close(sock);

    Serve_Stop(sctx);
    MemCtx_Free(m);
    return r;
}

status ServeHandle_Tests(MemCtx *gm){
    status r = TEST_OK;
    MemCtx *m = MemCtx_Make();

    Handler *h = NULL;
    Req *req = NULL;;
    ProtoDef *def = HttpProtoDef_Make(m);
    def->getHandler = Example_getHandler;
    Serve *sctx = Serve_Make(m, def);
    
    r |=  Serve_PreRun(sctx, TEST_PORT);

    int sock = testConnect();
    send(sock, reqStart_cstr, strlen(reqStart_cstr), 0);
    Serve_AcceptRound(sctx);

    Serve_ServeRound(sctx);
    req = sctx->active;
    r |= Test(req != NULL, "active Req is not null");
    
    h = Handler_Current(req->handler);
    r |= Test(h->func == Example_Setup, "Req has the first handler set");

    Serve_ServeRound(sctx);
    h = Handler_Current(req->handler);
    r |= Test(h->func == Example_Recieve, "Req has the second handler set");
    r |= Test(!HasFlag(req->in.rbl->type.state, SUCCESS), "Req Roebling does not have state SUCCESS yet, have %s", State_ToString(req->in.rbl->type.state));

    send(sock, reqEnd_cstr, strlen(reqEnd_cstr), 0);
    Serve_ServeRound(sctx);
    r |= Test(req->in.rbl->type.state == SUCCESS, "Req Roebling has status of SUCCESS, have %s", State_ToString(req->in.rbl->type.state));

    Serve_Stop(sctx);

    MemCtx_Free(m);
    return r;
}
