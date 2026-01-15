status WwwRoute_Tests(MemCh *gm);
status WwwRouteTempl_Tests(MemCh *gm);
status WwwRouteFmt_Tests(MemCh *m);
status WwwRouteMime_Tests(MemCh *gm);
status WwwRouteRbs_Tests(MemCh *m);
status WwwPath_Tests(MemCh *gm);
status WwwNav_Tests(MemCh *m);
status Http_Tests(MemCh *m);
status HttpQuery_Tests(MemCh *m);
status Inet_Tests(MemCh *gm);
status ServeTcp_Tests(MemCh *gm);
status Templ_Tests(MemCh *gm);
status TemplCtx_Tests(MemCh *gm);
status TemplNav_Tests(MemCh *gm);
status TemplNavNested_Tests(MemCh *m);
status Doc_Tests(MemCh *m);

static TestSet InterTests[] = {
        {
            "Www Nav Tests",
            WwwNav_Tests,
            "Example navigation generation tests.",
            FEATURE_COMPLETE,
        },
        {
            "Templ Navigation Tests",
            TemplNav_Tests,
            "Testing a navitaion template.",
            FEATURE_COMPLETE,
        },
    {
        "TemplCtx Tests",
        TemplCtx_Tests,
        "TemplCtx tests of basic value templating.",
        FEATURE_COMPLETE,
    },
    {
        "Templ Tests",
        Templ_Tests,
        "Testing templating with a few variables.",
        FEATURE_COMPLETE,
    },
    {
        "Templ Nested Navigation Tests",
        TemplNavNested_Tests,
        "Testing a nested navitaion template.",
        FEATURE_COMPLETE,
    },
    {
        "Inet Tests",
        Inet_Tests,
        "Inet address translation tests.",
        FEATURE_COMPLETE,
    },
    {
        "Task/Serve Tests",
        NULL,
        NULL,
        SECTION_LABEL,
    },
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
        "Www Nav Tests",
        WwwNav_Tests,
        "Example navigation generation tests.",
        FEATURE_COMPLETE,
    },
    {
        "Doc Tests",
        Doc_Tests,
        "Documentation generator tests.",
        FEATURE_COMPLETE|SKIP_TEST,
    },
    {
        NULL,
        NULL,
        NULL,
        0,
    }
};
