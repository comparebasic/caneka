status Http_Tests(MemCh *m);
status HttpQuery_Tests(MemCh *m);
status Inet_Tests(MemCh *gm);
status ServeTcp_Tests(MemCh *gm);

static TestSet InterTests[] = {
    {
        "Inet Tests",
        Inet_Tests,
        "Inet address translation tests.",
        FEATURE_COMPLETE,
    },
    {
        "Http Tests",
        Http_Tests,
        "Http Protocol and header parsing tests.",
        FEATURE_COMPLETE,
    },
    {
        "HttpQuery Tests",
        HttpQuery_Tests,
        "Http query string parsing and header parsing tests.",
        FEATURE_COMPLETE,
    },
    {
        NULL,
        NULL,
        NULL,
        0,
    }
};
