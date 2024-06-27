#include <external.h>
#include <caneka.h>
#include <testsuite.h>

#include <proto/http.h>

static char *sid = "Azjfhuei3";
static char *req_cstr = "GET /Azjfhuei3 HTTP/1.1\r\nContent-Length: 9\r\nHost: test.example.com\r\n\r\n{\"id\":23}";  

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
    HttpProto_Init(m);

    Serve *sctx = Serve_Make(m, HttpProtoDef_Make(m));
    
    r |=  Serve_PreRun(sctx, TEST_PORT);

    int sock = testConnect();
    Serve_AcceptRound(sctx);
    r |= Test(sock >= 0, "Expect socket to connect has fd %d", sock);
    send(sock, req_cstr, strlen(req_cstr), 0);
    Serve_ServeRound(sctx);
    close(sock);

    Serve_Stop(sctx);
    MemCtx_Free(m);
    return r;
}
