#include <external.h>
#include <caneka.h>

typedef struct test_capture {
    word captureKey;
    byte *content;
} TestCapture;

static char *captureKey_ToChars(word captureKey){
    if(captureKey == HTTP_METHOD){
        return "HTTP_METHOD";
    }else if(captureKey == HTTP_PATH){
        return "HTTP_PATH";
    }else if(captureKey == HTTP_PROTO){
        return "HTTP_PROTO";
    }else if(captureKey == HTTP_HEADER){
        return "HTTP_HEADER";
    }else if(captureKey == HTTP_HEADER_VALUE){
        return "HTTP_HEADER_VALUE";
    }else if(captureKey == HTTP_END){
        return "HTTP_END";
    }
    return "uknown";
}

static status r = READY;
static TestCapture *toCapture = NULL;
static int position = 0;

void setTestCapture(TestCapture *capture){
    toCapture = capture;
    position = 0;
}

static status testHttpParser_Capture(word captureKey, int matchIdx, String *s, Abstract *source){
    if(DEBUG_HTTP){
        printf("%d\n", captureKey);
        Debug_Print(s, 0, "TestCaptured: ", DEBUG_HTTP, TRUE);
        printf("\n");
    }

    TestCapture *exp = toCapture+position;

    word expectedKey = exp->captureKey;
    byte *b = exp->content;

    r |= Test(captureKey == expectedKey, "captureKey is as expected %s, have %d/%s", captureKey_ToChars(expectedKey), captureKey, captureKey_ToChars(captureKey));
    if(b != NULL){
        r |= Test(String_EqualsBytes(s, b), "Expected content '%s', have '%s'", b, String_ToEscaped(DebugM, s)->bytes);
    }
    position++;
    return NOOP;
}

TestCapture expected1[] = {
    {HTTP_METHOD, bytes("GET")},
    {HTTP_PATH, bytes("/path.html")},
    {HTTP_PROTO, bytes("HTTP/1.1")},
    {HTTP_HEADER, bytes("Host")},
    {HTTP_HEADER_VALUE, bytes("localhost")},
    {HTTP_HEADER, bytes("User-Agent")},
    {HTTP_HEADER_VALUE, bytes("Mozilla/5.0 (X11; Linux x86_64; rv:128.0) Gecko/20100101 Firefox/128.0")},
    {HTTP_HEADER, bytes("Connection")},
    {HTTP_HEADER_VALUE, bytes("keep-alive")},
    {HTTP_HEADER, bytes("Accept")},
    {HTTP_HEADER_VALUE, bytes("text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/png,image/svg+xml,*/*;q=0.8")},
    {HTTP_END, NULL},
    {0, NULL}
};

status Http_Tests(MemCtx *gm){
    r = TEST_OK;
    MemCtx *m = MemCtx_Make();

    String *s = NULL;
    s = String_Make(m, bytes("GET /path.html HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:128.0) Gecko/20100101 Firefox/128.0\r\n"
        "Connection: keep-alive\r\n"
        "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/png,image/svg+xml,*/*;q=0.8\r\n"
        "\r\n"
    ));

    ProtoDef *def = (ProtoDef *)HttpProtoDef_Make(m);
    Proto *proto = HttpProto_Make(m, def);
    Roebling *rbl = HttpParser_Make(m, s, (Abstract *)proto);
    rbl->capture = testHttpParser_Capture;

    setTestCapture(expected1);
    for(int i = 0; i < 13; i++){
        Roebling_RunCycle(rbl);
        /*
        Debug_Print((void *)rbl, 0, "Rbl: ", COLOR_PURPLE, TRUE);
        */
    }

    r |= Test(position == 12, "Reached end of test captures, have %d", position);
    r |= Test(HasFlag(rbl->type.state, SUCCESS), "Roebling HttpParser has state SUCCESS, have '%s'", State_ToChars(rbl->type.state));

    MemCtx_Free(m);

    return r;
}

