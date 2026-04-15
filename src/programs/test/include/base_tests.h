status Stream_Tests(MemCh *gm);
status Iter_Tests(MemCh *gm);
status IterMax_Tests(MemCh *gm);
status IterPrevRemove_Tests(MemCh *gm);
status Encoding_Tests(MemCh *gm);
status Buff_Tests(MemCh *gm);
status BuffIo_Tests(MemCh *gm);
status BuffPos_Tests(MemCh *gm);
status BuffSendRecv_Tests(MemCh *gm);
status Time_Tests(MemCh *gm);
status Table_Tests(MemCh *gm);
status TableResize_Tests(MemCh *gm);
status TableUtilKey_Tests(MemCh *gm);
status TableHKey_Tests(MemCh *gm);
status StrVec_Tests(MemCh *gm);
status Stash_Tests(MemCh *gm);
status Str_Tests(MemCh *gm);
status Str_EndMatchTests(MemCh *gm);
status Span_Tests(MemCh *gm);
status SpanClone_Tests(MemCh *gm);
status SpanMax_Tests(MemCh *gm);
status Path_Tests(MemCh *gm);
status MemCh_Tests(MemCh *gm);
status MemChLevel_Tests(MemCh *gm);
status Hash_Tests(MemCh *gm);
status Cursor_Tests(MemCh *gm);
status Core_Tests(MemCh *gm);
status Clone_Tests(MemCh *gm);
status DebugStack_Tests(MemCh *gm);
status Histo_Tests(MemCh *m);
status Parity_Tests(MemCh *m);

static TestSet BaseTests[] = {
    {
        "Basic Features",
        NULL,
        NULL,
        SECTION_LABEL,
    },
    {
        "Core",
        Core_Tests,
        "Initializing core system tests",
        FEATURE_COMPLETE,
    },
    {
        "MemCh Tests",
        MemCh_Tests,
        "Testing the underlying memory manager to ensure leaks have not occured in specific cases..",
        FEATURE_COMPLETE,
    },
    {
        "MemCh Level Tests",
        MemChLevel_Tests,
        "Testing adding a remove memory on levels of a Memory Chapter",
        FEATURE_COMPLETE,
    },
    {
        "Span",
        Span_Tests,
        "Span is the basic data structure for sequential or logorithmic storage access.",
        FEATURE_COMPLETE,
    },
#ifdef MEM_MAX_TESTS
    {
        "Span Max Tests",
        SpanMax_Tests,
        "Populate a span with the maximum amount of values to stress test it and the memory system.",
        FEATURE_COMPLETE,
    },
#endif
    {
        "Iter",
        Iter_Tests,
        "Iter tests for iteration and insertion.",
        FEATURE_COMPLETE,
    },
#ifdef MEM_MAX_TESTS
    {
        "IterMax",
        IterMax_Tests,
        "Iter tests for iteration over increasingly large sets",
        FEATURE_COMPLETE,
    },
    {
        "IterPrevRemove",
        IterPrevRemove_Tests,
        "Iter tests for previous removal over increasingly large sets.",
        FEATURE_COMPLETE,
    },
#endif
    {
        "Str",
        Str_Tests,
        "Tests for the basic string object and convienience features",
        FEATURE_COMPLETE,
    },
    {
        "StrVec",
        StrVec_Tests,
        "Testing String Vectors",
        FEATURE_COMPLETE,
    },
    {
        "Histo Tests",
        Histo_Tests,
        "Text content histogram testing, to prevent code or sql injection.",
        FEATURE_COMPLETE,
    },
    {
        "String - End Matches",
        Str_EndMatchTests,
        "Tests for Matching the end of the string",
        FEATURE_COMPLETE,
    },
    {
        "Encoding Tests",
        Encoding_Tests,
        "Encoding tests for hex.",
        FEATURE_COMPLETE,
    },
    {
        "Buff Tests",
        Buff_Tests,
        "Testing the buffer add functionality.",
        FEATURE_COMPLETE,
    },
    {
        "Buff Send/Recv Tests",
        BuffSendRecv_Tests,
        "Testing the buffer send and recv components.",
        FEATURE_COMPLETE,
    },
    {
        "Buff IO Tests",
        BuffIo_Tests,
        "Testing the buffer send and recv to disk/socket components.",
        FEATURE_COMPLETE,
    },
    {
        "Buff Position Tests",
        BuffPos_Tests,
        "Testing the buffer position features of seeking and moving around it.",
        FEATURE_COMPLETE,
    },
    {
        "Buff Stream Tests",
        Stream_Tests,
        "Testing setting up and running multiple stream types and stream chains",
        FEATURE_COMPLETE,
    },
    {
        "Cursor Tests",
        Cursor_Tests,
        "Cursor tests for adding and navigation the position of a StrVec",
        FEATURE_COMPLETE,
    },
    {
        "DebugStack Tests",
        DebugStack_Tests,
        "Testing DebugStack crash debugging system",
        FEATURE_COMPLETE,
    },
    {
        "Parity Tests",
        Parity_Tests,
        "A simple 4 byte sum + 7-bit content length check utility tests.",
        FEATURE_COMPLETE,
    },
    {
        "Hash",
        Hash_Tests,
        "Tests for the Hash object which used for table storage and comparison of a few Polymorphic data types.",
        FEATURE_COMPLETE,
    },
    {
        "Table HKey Tests",
        TableHKey_Tests,
        "Testing table HKey component which determines the order of indicies to look for an item.",
        FEATURE_COMPLETE,
    },
    {
        "Table",
        Table_Tests,
        "Auto-scaling hash-table built on top of the Span object and Hash object.",
        FEATURE_COMPLETE,
    },
    {
        "TableResize",
        TableResize_Tests,
        "Resize features of the auto-scaling hash-table.",
        FEATURE_COMPLETE,
    },
    {
        "TableUtilKey",
        TableUtilKey_Tests,
        "Testing table with a util wrapped instead of string keys.",
        FEATURE_COMPLETE,
    },
    {
        "Clone",
        Clone_Tests,
        "Clone test of cloning basic types.",
        FEATURE_COMPLETE,
    },
    {
        "Time",
        Time_Tests,
        "Time tests for converstion between unix syscalls and struct timespec and ApproxTime.",
        FEATURE_COMPLETE,
    },
    {
        "Persistance Tests",
        NULL,
        NULL,
        SECTION_LABEL,
    },
    {
        "Path Tests",
        Path_Tests,
        "Testing path functions for file and object variable paths.",
        FEATURE_COMPLETE,
    },
    {
        "Stash Tests",
        Stash_Tests,
        "Mem Stash testing.",
        FEATURE_COMPLETE,
    },
    {
        NULL,
        NULL,
        NULL,
        0,
    }
};
