status WwwRoute_Tests(MemCh *gm);
status WwwRouteTempl_Tests(MemCh *gm);
status WwwRouteFmt_Tests(MemCh *m);
status WwwRouteMime_Tests(MemCh *gm);
status WwwRouteRbs_Tests(MemCh *m);
status WwwPath_Tests(MemCh *gm);
status Http_Tests(MemCh *m);
status HttpQuery_Tests(MemCh *m);
status Session_Tests(MemCh *m);
status Login_Tests(MemCh *m);

static TestSet InterTests[] = {
    {
        "Www Tests",
        NULL,
        NULL,
        SECTION_LABEL,
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
        "Www Route Tests",
        WwwRoute_Tests,
        "Testing an object that gathers and caches routes for web servers.",
        FEATURE_COMPLETE|SKIP_TEST,
    },
    {
        "Www Route Template Tests",
        WwwRouteTempl_Tests,
        "Example route templating test.",
        FEATURE_COMPLETE|SKIP_TEST,
    },
    {
        "Www Route Template Tests",
        WwwRouteFmt_Tests,
        "Example route templating Pencil Format test.",
        FEATURE_COMPLETE|SKIP_TEST,
    },
    {
        "Www Route RevBinSeg Tests",
        WwwRouteRbs_Tests,
        "Testing submission to an RevBinSeg DB",
        FEATURE_COMPLETE|SKIP_TEST,
    },
    {
        "Www Path Tests",
        WwwPath_Tests,
        "Example route path templating test.",
        FEATURE_COMPLETE|SKIP_TEST,
    },
    {
        "Www Route Mime Tests",
        WwwRouteMime_Tests,
        "Example route mime handling tests using Route_Handle.",
        FEATURE_COMPLETE|SKIP_TEST,
    },
    {
        "Session Tests",
        Session_Tests,
        "Session Open/Close tests.",
        FEATURE_COMPLETE,
    },
    {
        "Login Tests",
        Login_Tests,
        "User login tests.",
        FEATURE_COMPLETE,
    },
    {
        NULL,
        NULL,
        NULL,
        0,
    }

};
