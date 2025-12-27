status BinSeg_Tests(MemCh *m);
status BinSegCollection_Tests(MemCh *m);
status BinSegV_Tests(MemCh *m);
status BinSegReversedV_Tests(MemCh *m);
status Config_Tests(MemCh *m);
status TaskStep_Tests(MemCh *m);
status NodeObj_Tests(MemCh *m);
status StrVecSplit_Tests(MemCh *m);
status Snip_Tests(MemCh *m);
status SnipModify_Tests(MemCh *m);
status QueueIter_Tests(MemCh *m);
status QueueCriteria_Tests(MemCh *m);
status QueueScale_Tests(MemCh *m);
status Queue_Tests(MemCh *m);
status QueueAddRemove_Tests(MemCh *m);
status Relation_Tests(MemCh *m);
status Object_Tests(MemCh *m);
status ObjectFilter_Tests(MemCh *m);
status Mess_Tests(MemCh *m);
status PatChar_Tests(MemCh *m);
status Roebling_Tests(MemCh *m);
status RoeblingRun_Tests(MemCh *m);
status RoeblingMark_Tests(MemCh *m);
status RoeblingStartStop_Tests(MemCh *m);
status Match_Tests(MemCh *m);
status MatchElastic_Tests(MemCh *m);
status MatchKo_Tests(MemCh *m);
status MatchReplace_Tests(MemCh *m);
status FmtHtml_Tests(MemCh *m);
status Doc_Tests(MemCh *m);
#ifdef CNKOPT_FREEBSD
status SysQuery_Tests(MemCh *m);
#endif

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
    /*
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
    */
    {
        "TaskStep Tests",
        TaskStep_Tests,
        "Task/Step and Task_Tumble tests.",
        FEATURE_COMPLETE,
    },
#ifdef CNKOPT_FREEBSD
    {
        "SysQuery Tests",
        SysQuery_Tests,
        "System Queries for Information such as Memory and CPU Performance.",
        FEATURE_COMPLETE,
    },
#endif
    {
        "Doc Tests",
        Doc_Tests,
        "Documentation generator tests.",
        FEATURE_COMPLETE,
    },


    {
        NULL,
        NULL,
        NULL,
        0,
    }
};
