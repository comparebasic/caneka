status BinSeg_Tests(MemCh *gm);
status BinSegCollection_Tests(MemCh *gm);
status BinSegV_Tests(MemCh *gm);
status BinSegReversedV_Tests(MemCh *gm);
status Config_Tests(MemCh *gm);
status TaskStep_Tests(MemCh *gm);
status Templ_Tests(MemCh *gm);
status TemplCtx_Tests(MemCh *gm);
status NodeObj_Tests(MemCh *gm);
status StrVecSplit_Tests(MemCh *gm);
status ServeTcp_Tests(MemCh *gm);
status Snip_Tests(MemCh *gm);
status SnipModify_Tests(MemCh *gm);
status Inet_Tests(MemCh *gm);
status Password_Tests(MemCh *m);
status QueueIter_Tests(MemCh *gm);
status QueueCriteria_Tests(MemCh *gm);
status QueueScale_Tests(MemCh *gm);
status Queue_Tests(MemCh *gm);
status QueueAddRemove_Tests(MemCh *gm);
status Relation_Tests(MemCh *gm);
status Object_Tests(MemCh *gm);
status ObjectFilter_Tests(MemCh *gm);
status Mess_Tests(MemCh *gm);
status PatChar_Tests(MemCh *gm);
status Roebling_Tests(MemCh *gm);
status RoeblingRun_Tests(MemCh *gm);
status RoeblingMark_Tests(MemCh *gm);
status RoeblingStartStop_Tests(MemCh *gm);
status Match_Tests(MemCh *gm);
status MatchElastic_Tests(MemCh *gm);
status MatchKo_Tests(MemCh *gm);
status MatchReplace_Tests(MemCh *gm);
status FmtHtml_Tests(MemCh *gm);

static TestSet ExtTests[] = {
    {
        "Object Tests",
        Object_Tests,
        "Object tests of ordered table of add/get.",
        FEATURE_COMPLETE,
    },
    {
        "Object Filter Tests",
        ObjectFilter_Tests,
        "Object filter tests.",
        FEATURE_COMPLETE,
    },
    {
        "Basic Features",
        NULL,
        NULL,
        SECTION_LABEL,
    },
    {
        "BinSeg Tests",
        BinSeg_Tests,
        "Transactional format encoding capable of representing stream, node, and database content.",
        FEATURE_COMPLETE,
    },
    {
        "BinSegCollection Tests",
        BinSegCollection_Tests,
        "Transactional format storing and reading collection and dictionary data types.",
        FEATURE_COMPLETE,
    },
    {
        "Roebling Parser",
        NULL,
        NULL,
        SECTION_LABEL,
    },
    {
        "Pattern Character Tests",
        PatChar_Tests,
        "Making patterns from strings.",
        FEATURE_COMPLETE,
    },
    {
        "Match Tests",
        Match_Tests,
        "Matching various patterns.",
        FEATURE_COMPLETE,
    },
    {
        "Match Elastic Tests",
        MatchElastic_Tests,
        "Testing matches on ranges that have variable or elastic lengths.",
        FEATURE_COMPLETE,
    },
    {
        "Match Knockout Tests",
        MatchKo_Tests,
        "Testing knockout patterns in match definitions.",
        FEATURE_COMPLETE,
    },
    {
        "Match Replace Tests",
        MatchReplace_Tests,
        "Tests for Str replaces with Match",
        FEATURE_COMPLETE,
    },
    {
        "StrVecSplit via Match",
        StrVecSplit_Tests,
        "Testing String Vector Split function",
        FEATURE_COMPLETE,
    },
    {
        "Snip Tests",
        Snip_Tests,
        "Snip tests for capturing a segment of a cursor",
        FEATURE_COMPLETE,
    },
    {
        "Snip Modify Tests",
        SnipModify_Tests,
        "Snip tests for modifying an existing captur in progress",
        FEATURE_COMPLETE,
    },
    {
        "Roebling Init Test",
        Roebling_Tests,
        "Roebling tests for adding and running a basic parser.",
        FEATURE_COMPLETE,
    },
    {
        "Roebling Run Tests",
        RoeblingRun_Tests,
        "Roebling basic run tests.",
        FEATURE_COMPLETE,
    },
    {
        "Roebling Mark Tests",
        RoeblingMark_Tests,
        "Roebling mark tests.",
        FEATURE_COMPLETE,
    },
    {
        "Roebling Start Stop Tests",
        RoeblingStartStop_Tests,
        "Roebling start and stop in the middle of recieving input tests.",
        FEATURE_COMPLETE,
    },
    /*
    {
        "Relation Tests",
        Relation_Tests,
        "Tests for the relation structure.",
        FEATURE_COMPLETE,
    },
    */
    {
        "Formats",
        NULL,
        NULL,
        SECTION_LABEL,
    },
    {
        "Mess Tests",
        Mess_Tests,
        "From a basic Fmt Roebling parser into the Mess data structures.",
        FEATURE_COMPLETE,
    },
    {
        "FmtHtml Tests",
        FmtHtml_Tests,
        "Format FMT -> HTML tests.",
        FEATURE_COMPLETE,
    },
    {
        "NodeObj Tests",
        NodeObj_Tests,
        "Node Object nested path tests.",
        FEATURE_COMPLETE,
    },
    {
        "Config Tests",
        Config_Tests,
        "Config tests that turn a basic configuration format into an object",
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
        "Task/Serve Tests",
        NULL,
        NULL,
        SECTION_LABEL,
    },
    {
        "Queue Tests",
        Queue_Tests,
        "Queue tests for file descriptors.",
        FEATURE_COMPLETE,
    },
    {
        "Queue Add Remove Tests",
        QueueAddRemove_Tests,
        "Queue tests for adding and removing items.",
        FEATURE_COMPLETE,
    },
    {
        "Queue Iter Tests",
        QueueIter_Tests,
        "Queue tests for iterating around a queue with additions and removals.",
        FEATURE_COMPLETE,
    },
    {
        "Queue Criteria Tests",
        QueueCriteria_Tests,
        "Queue tests for multi-critiria items (time/file descriptor).",
        FEATURE_COMPLETE,
    },
    {
        "Queue Scale Tests",
        QueueScale_Tests,
        "Queue testing adding and removing Queue items over time.",
        FEATURE_COMPLETE,
    },
    {
        "TaskStep Tests",
        TaskStep_Tests,
        "Task/Step and Task_Tumble tests.",
        FEATURE_COMPLETE,
    },
    {
        "Inet Tests",
        Inet_Tests,
        "Inet address translation tests.",
        FEATURE_COMPLETE,
    },
    {
        "Password Tests",
        Password_Tests,
        "Password tests with variable salt and funny.",
        FEATURE_COMPLETE,
    },
    {
        NULL,
        NULL,
        NULL,
        0,
    }
};
