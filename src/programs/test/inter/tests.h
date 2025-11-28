static TestSet InterTests[] = {
    {
        "Http Tests",
        Http_Tests,
        "Http Protocol and header parsing tests.",
        FEATURE_COMPLETE,
    },
    {
        "Www Tests",
        NULL,
        NULL,
        SECTION_LABEL,
    },
    {
        "Www Route Tests",
        WwwRoute_Tests,
        "Testing an object that gathers and caches routes for web servers.",
        FEATURE_COMPLETE,
    },
    {
        "Www Route Template Tests",
        WwwRouteTempl_Tests,
        "Example route templating test.",
        FEATURE_COMPLETE,
    },
    {
        "Www Route Template Tests",
        WwwRouteFmt_Tests,
        "Example route templating Pencil Format test.",
        FEATURE_COMPLETE,
    },
    {
        "Www Path Tests",
        WwwPath_Tests,
        "Example route path templating test.",
        FEATURE_COMPLETE,
    },
    {
        "Www Route Mime Tests",
        WwwRouteMime_Tests,
        "Example route mime handling tests using Route_Handle.",
        FEATURE_COMPLETE,
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

}

status WwwRoute_Tests(MemCh *gm);
status WwwRouteTempl_Tests(MemCh *gm);
status WwwRouteFmt_Tests(MemCh *m);
status WwwRouteMime_Tests(MemCh *gm);
status WwwPath_Tests(MemCh *gm);
status Http_Tests(MemCh *m);
status Session_Tests(MemCh *m);
status Login_Tests(MemCh *m);
