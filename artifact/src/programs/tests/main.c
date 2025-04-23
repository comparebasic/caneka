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
        "StrVec",
        StrVec_Tests,
        "Testing String Vectors",
        FEATURE_COMPLETE,
    },
    {
        "Str",
        Str_Tests,
        "Tests for the basic string object and convienience features",
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
        "Roebling Tesst",
        Roebling_Tests,
        "Roebling tests for adding and running a basic parser.",
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
        Fatal(0, FUNCNAME, FILENAME, LINENUMBER, "MemBook created successfully", NULL);
    }

    MemCh *m = MemCh_Make();
    if(m == NULL){
        Fatal(0, FUNCNAME, FILENAME, LINENUMBER, "MemCh created successfully", NULL);
    }

    Caneka_Init(m);
    test(m);

    return 0;
}
