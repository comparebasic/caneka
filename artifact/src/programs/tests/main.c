#include <external.h>
#include <caneka.h>
#include <tests.h>

static status test(MemCh *m){
    status r = READY;
    Tests_Init(m);
    r |= Test_Runner(m, "Caneka", Tests);
    return r;
}

status Tests_Init(MemCh *m){
   status r = READY;
   r |= Make109Strs(m);
   return r;
}

static TestSet _Tests[] = {
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
        "Span",
        Span_Tests,
        "Span is the basic data structure for sequential or logorithmic storage access.",
        FEATURE_COMPLETE,
    },
    {
        "Iter",
        Iter_Tests,
        "Iter tests for iteration and insertion.",
        FEATURE_COMPLETE,
    },
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
        "String - End Matches",
        Str_EndMatchTests,
        "Tests for Matching the end of the string",
        FEATURE_COMPLETE,
    },
    {
        "Stream Tests",
        Stream_Tests,
        "Testing setting up and running multiple stream types and stream chains",
        FEATURE_COMPLETE,
    },
    {
        "DebugStack Tests",
        DebugStack_Tests,
        "Testing DebugStack crash debugging system",
        FEATURE_COMPLETE,
    },
    {
        "Hash",
        Hash_Tests,
        "Tests for the Hash object which used for table storage and comparison of a few Polymorphic data types.",
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
        "Clone",
        Clone_Tests,
        "Clone test of cloning basic types.",
        FEATURE_COMPLETE,
    },
    {
        "Roebling Parser",
        NULL,
        NULL,
        SECTION_LABEL,
    },
    {
        "Patter Character Tests",
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
        "Cursor Tests",
        Cursor_Tests,
        "Cursor tests for adding and navigation the position of a StrVec",
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
    {
        "Relation Tests",
        Relation_Tests,
        "Tests for the relation structure.",
        FEATURE_COMPLETE,
    },
    {
        "BinPin Format Tests",
        BinPin_Tests,
        "Small example binpin format printout",
        FEATURE_COMPLETE,
    },
    {
        "Mess Tests",
        Mess_Tests,
        "From a basic Fmt Roebling parser into the Mess data structures.",
        FEATURE_COMPLETE,
    },
    {
        "File Tests",
        File_Tests,
        "File handling tests.",
        FEATURE_COMPLETE,
    },
    {
        "FmtHtml Tests",
        FmtHtml_Tests,
        "Format FMT -> HTML tests.",
        FEATURE_COMPLETE,
    },
    {
        "OrdTable Tests",
        OrdTable_Tests,
        "OrdTable tests of ordered table of add/get.",
        FEATURE_COMPLETE,
    },
    {
        "Cash Tests",
        Cash_Tests,
        "Cash tests of basic value templating.",
        FEATURE_COMPLETE,
    },
    {
        "CashTempl Tests",
        CashTempl_Tests,
        "Cash tests of basic logic templating.",
        FEATURE_COMPLETE,
    },
    {
        "Path Tests",
        Path_Tests,
        "Testing path functions for file and object variable paths.",
        FEATURE_COMPLETE,
    },
    {
        "Nested Tests",
        Nested_Tests,
        "Testing nested component, which manages nested OrdTables and Spans.",
        FEATURE_COMPLETE,
    },
    {
        "Templ Tests",
        Templ_Tests,
        "Testing templating a Cash template.",
        FEATURE_COMPLETE,
    },
    {
        "TemplLogic Tests",
        TemplLogic_Tests,
        "Testing templating logic tags in a Cash template.",
        FEATURE_COMPLETE,
    },
    /*
    {
        "TablePreKey",
        TablePreKey_Tests,
        "Testing the ability to add a key and value to a hash table and disperate, yet sequential, times.",
        FEATURE_COMPLETE,
    },
    {
        "String - B64",
        StringB64_Tests,
        "Tests base 64 encoded strings",
        FEATURE_COMPLETE,
    },
    */
    {
        NULL,
        NULL,
        NULL,
        0,
    },
};

TestSet *Tests = _Tests;

i32 main(int argc, char **argv){
    if(argc > 1){
        for(int i = 1; i < argc; i++){
            char *arg = argv[i];
            if(strncmp(arg, "no-color", strlen("no-color")) == 0){
                GLOBAL_flags |= NO_COLOR;
            }
            if(strncmp(arg, "html", strlen("html")) == 0){
                GLOBAL_flags |= HTML_OUTPUT;
            }
        }
    }

    MemBook *cp = MemBook_Make(NULL);
    if(cp == NULL){
        Fatal(FUNCNAME, FILENAME, LINENUMBER, "MemBook created successfully", NULL);
    }

    MemCh *m = MemCh_Make();
    if(m == NULL){
        Fatal(FUNCNAME, FILENAME, LINENUMBER, "MemCh created successfully", NULL);
    }

    Caneka_Init(m);
    DebugStack_Push(NULL, 0);
    test(m);

    DebugStack_Pop();
    return 0;
}
