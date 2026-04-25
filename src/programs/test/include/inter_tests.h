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
        NULL,
        NULL,
        NULL,
        0,
    }
};
