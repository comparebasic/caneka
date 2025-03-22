#include <external.h>
#include <caneka.h>
#include <tests.h>

#include "./mock_109strings.c"

status Tests_Init(MemCtx *m){
   status r = READY;
   r |= Make109Strings(m);
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
        "String",
        String_Tests,
        "Tests for the basic string object and convienience features",
        FEATURE_COMPLETE,
    },
    {
        "String - End Matches",
        String_EndMatchTests,
        "Tests for Matching the end of the string",
        FEATURE_COMPLETE,
    },
    {
        "String - B64",
        StringB64_Tests,
        "Tests base 64 encoded strings",
        FEATURE_COMPLETE,
    },
    {
        "Span",
        Span_Tests,
        "Span is the basic data structure for sequential or logorithmic storage access.",
        FEATURE_COMPLETE,
    },
    {
        "MemCtx Tests",
        MemCtx_Tests,
        "Testing the underlying memory manager to ensure leaks have not occured in specific cases..",
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
        "TablePreKey",
        TablePreKey_Tests,
        "Testing the ability to add a key and value to a hash table and disperate, yet sequential, times.",
        FEATURE_COMPLETE,
    },
    {
        NULL,
        NULL,
        NULL,
        0,
    },
};

TestSet *Tests = _Tests;
