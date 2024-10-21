#include <external.h>
#include <caneka.h>
#include <example.h>

static char *sid = "Azjfhuei3";
static char *req_cstr = "GET /home HTTP/1.1\r\nContent-Length: 9\r\nHost: test.example.com\r\n\r\n{\"id\":23}";  

static char *req1_cstr = "GET /home HTTP/1.1\r\nContent-Leng";  
static char *req2_cstr = "th: 9\r\nHost: test.example.com\r\n\r\n{\"id\":23}";  

status Serve_Tests(MemCtx *gm){
    status r = TEST_OK;
    MemCtx *m = MemCtx_Make();

    ProtoDef *def = HttpProtoDef_Make(m);
    def->getHandler = Example_getHandler;
    Serve *sctx = Serve_Make(m, def);
    
    r |=  Serve_PreRun(sctx, TEST_PORT);

    ReqTestSpec specs[] = {
        {0, NULL, 0,  1, 0},
        {TEST_SEND, req_cstr, strlen(req_cstr), 1, 0},
        {TEST_SERVE_END, NULL, 0, 0,  0}
    };
    int child = ServeTests_ForkRequest(m, "Connect test", specs);
    Delay();
    Delay();
    Delay();
    Delay();
    Delay();
    Delay();
    Serve_AcceptRound(sctx);
    Delay();
    Delay();
    Serve_ServeRound(sctx);

    r |= TestChild(child);

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

    ReqTestSpec specs[] = {
        {TEST_SEND, req_cstr, strlen(req_cstr), 1, 0},
        {TEST_SERVE_END, NULL, 0, 0,  0}
    };
    int child = ServeTests_ForkRequest(m, "Handler test", specs);
    Delay();
    Delay();
    Serve_AcceptRound(sctx);

    Serve_ServeRound(sctx);
    req = sctx->active;
    r |= Test(req != NULL, "active Req is not null");
    
    h = Handler_Current(req->handler);
    r |= Test(h->func == Example_Setup, "Req has the first handler set");

    Serve_ServeRound(sctx);
    h = Handler_Current(req->handler);
    r |= Test(h->func == Example_Recieve, "Req has the second handler set");

    Serve_ServeRound(sctx);
    Serve_ServeRound(sctx);
    Serve_ServeRound(sctx);
    r |= Test(req->in.rbl->type.state == SUCCESS, "Req Roebling has status of SUCCESS, have %s", State_ToString(req->in.rbl->type.state));

    int st = 0;
    pid_t pid = waitpid(child, &st, 0);
    int code = WEXITSTATUS(st);
    r |= Test(code == 0, "Proc %d existed with status %d", pid, code);

    Serve_Stop(sctx);

    MemCtx_Free(m);
    return r;
}

status ServeChunked_Tests(MemCtx *gm){
    status r = TEST_OK;
    MemCtx *m = MemCtx_Make();

    Handler *h = NULL;
    Req *req = NULL;;
    ProtoDef *def = HttpProtoDef_Make(m);
    def->getHandler = Example_getHandler;
    Serve *sctx = Serve_Make(m, def);
    
    r |=  Serve_PreRun(sctx, TEST_PORT);

    ReqTestSpec specs[] = {
        {TEST_SEND, req1_cstr, strlen(req1_cstr), 0, 0},
        {TEST_DELAY_ONLY, NULL, 0, 5, 0},
        {TEST_SEND, req2_cstr, strlen(req2_cstr), 0, 0},
        {TEST_SERVE_END, NULL, 0, 0,  0}
    };
    int child = ServeTests_ForkRequest(m, "Chunked test", specs);
    Delay();
    Delay();
    Serve_AcceptRound(sctx);

    Serve_ServeRound(sctx);
    req = sctx->active;
    r |= Test(req != NULL, "active Req is not null");
    
    h = Handler_Current(req->handler);
    r |= Test(h->func == Example_Setup, "Req has the first handler set");

    Delay();
    Serve_ServeRound(sctx);
    h = Handler_Current(req->handler);
    r |= Test(h->func == Example_Recieve, "Req has the second handler set");
    r |= Test(!HasFlag(req->in.rbl->type.state, SUCCESS), "Req Roebling does not have state SUCCESS yet, have %s", State_ToString(req->in.rbl->type.state));

    Delay();
    Delay();
    Delay();
    Delay();
    Delay();
    Delay();
    Serve_ServeRound(sctx);
    Serve_ServeRound(sctx);
    r |= Test(req->in.rbl->type.state == SUCCESS, "Req Roebling has status of SUCCESS, have %s", State_ToString(req->in.rbl->type.state));

    r |= TestChild(child);

    Serve_Stop(sctx);

    MemCtx_Free(m);
    return r;
}

#define MULTIPLE_COUNT 15
int atOncePids[MULTIPLE_COUNT] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,/*0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,*/
};

status ServeMultiple_Tests(MemCtx *gm){
    status r = TEST_OK;
    MemCtx *m = MemCtx_Make();

    Handler *h = NULL;
    Req *req = NULL;;
    ProtoDef *def = HttpProtoDef_Make(m);
    def->getHandler = Example_getHandler;
    Serve *sctx = Serve_Make(m, def);
    
    r |=  Serve_PreRun(sctx, TEST_PORT);

    ReqTestSpec specs[] = {
        {TEST_SEND, req1_cstr, strlen(req1_cstr), 0, 0},
        {TEST_DELAY_ONLY, NULL, 0, 16, 0},
        {TEST_SEND, req2_cstr, strlen(req2_cstr), 0, 0},
        {TEST_SERVE_END, NULL, 0, 0,  0}
    };

    for(int i = 0; i < MULTIPLE_COUNT; i++){
        String *s = String_Init(m, STRING_EXTEND);
        String_AddBytes(m, s, bytes("Multiple requests "), strlen("Multiple requests "));
        String_Add(m, s, String_FromInt(m, i));
        atOncePids[i] = ServeTests_ForkRequest(m, (char *)s->bytes, specs);
    }

    for(int i = 0; i < MULTIPLE_COUNT / 3; i++){
        Delay();
        Delay();
        Delay();
        Delay();
        Serve_AcceptRound(sctx);
    }

    r |= Test(sctx->metrics.open == MULTIPLE_COUNT, "Multiple count of %d currently open, have %d", MULTIPLE_COUNT, sctx->metrics.open);

    for(int i = 0; i < MULTIPLE_COUNT; i++){
        Delay();
    }

    for(int i = 0; i < 8; i++){
        Serve_ServeRound(sctx);
    }

    r |= Test(sctx->metrics.open == 0, "All processes closed, count of 0 currently open, have %d", sctx->metrics.open);
    r |= Test(sctx->metrics.served == MULTIPLE_COUNT, "Served all count of %d requests, have %d", sctx->metrics.served);

    for(int i = 0; i < MULTIPLE_COUNT; i++){
        r |= TestChild(atOncePids[i]);
    }

    Serve_Stop(sctx);

    MemCtx_Free(m);
    return r;
}
