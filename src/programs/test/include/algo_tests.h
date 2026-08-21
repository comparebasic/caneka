status MediTree_Tests(MemCh *m);

static TestSet AlgoTests[] = {
    {
        "Algo Features",
        NULL,
        NULL,
        SECTION_LABEL,
    },
    {
        "MediTree Tests",
        MediTree_Tests,
        "MediTree Tests of basic insert and retrieval",
        FEATURE_COMPLETE,
    },
    {
        NULL,
        NULL,
        NULL,
        0,
    }
};
