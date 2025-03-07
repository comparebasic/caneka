#include <external.h>
#include <caneka.h>
#include <tests/tests.h>

#define CYCLE_SERVE 1

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
        "SpanSetup",
        SpanSetup_Tests,
        "This tests the setup for a variety of Span configurations.",
        FEATURE_COMPLETE,
    },
    {
        "SpanInline",
        SpanInline_Tests,
        "Inline spans hold data directly in thier memory space, instead of storing pointers to data held elsewhere.",
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
        "IterStr",
        IterStr_Tests,
        "Iter Strings are strings that contain arrays of other items.",
        FEATURE_COMPLETE,
    },
    {
        "Roebling Parser",
        NULL,
        NULL,
        SECTION_LABEL,
    },
    {
        "Match",
        Match_Tests,
        "Character pattern matching system for the Roebling parser.",
        FEATURE_COMPLETE,
    },
    {
        "MatchElastic",
        MatchElastic_Tests,
        "Elastic, (length undetermined) match testing for character patterns of the Roebling parser.",
        FEATURE_COMPLETE,
    },
    {
        "MatchKo",
        MatchKo_Tests,
        "Testing the knockout and content termination capabilities of the match system.",
        FEATURE_COMPLETE,
    },
    {
        "StrSnip",
        StrSnip_Tests,
        "Test for snip ranges of strings to copy.",
        FEATURE_COMPLETE,
    },
    {
        "StrSnip Boundry",
        StrSnipBoundry_Tests,
        "Test for snip ranges of strings to copy in large strings.",
        FEATURE_COMPLETE,
    },
    {
        "Roebling",
        Roebling_Tests,
        "Tests for the Roebling character pattern matching parser, with programmable hooks and capture ranges.",
        FEATURE_COMPLETE,
    },
    {
        "RoeblingRun",
        RoeblingRun_Tests,
        "Run state variables for the Roebling parser.",
        FEATURE_COMPLETE,
    },
    {
        "RoeblingMark",
        RoeblingMark_Tests,
        "Mark and jump behavior tests for the Roebling parser.",
        FEATURE_COMPLETE,
    },
    {
        "RoeblingStartSop",
        RoeblingStartStop_Tests,
        "Pausable behavior for the Roebling parser, to be able to handle partial input, and resume when more input is added.",
        FEATURE_COMPLETE,
    },
    {
        "Roebling Formats",
        NULL,
        NULL,
        SECTION_LABEL,
    },
    {
        "XML",
        Xml_Tests,
        "Basic XML parsing implemented using the Roebling Parser",
        FEATURE_COMPLETE,
    },
    {
        "XMLNested",
        XmlNested_Tests,
        "Nested XML parsing implemented using the Roebling Parser",
        FEATURE_COMPLETE,
    },
    {
        "XMLParser",
        XmlParser_Tests,
        "Fully featured XML parsing implemented using the Roebling Parser",
        FEATURE_COMPLETE,
    },
    {
        "XMLStringType",
        XmlStringType_Tests,
        "Identify string types during parsing (NUM/CASH)",
        FEATURE_COMPLETE,
    },
    {
        "Http",
        Http_Tests,
        "HTTP protocol parsing using the Roebling Parser",
        FEATURE_COMPLETE,
    },
    {
        "TermIo - Roebling Format",
        TermIoFormat_Tests,
        "Digesting terminal output to consider control characters, such as color and motion.",
        FEATURE_COMPLETE,
    },
    {
        "Oset",
        Oset_Tests,
        "Oset serialization format tests (similar features to XML, JSON, or YAML).",
        FEATURE_COMPLETE,
    },
    {
        "Scheduling",
        NULL,
        NULL,
        SECTION_LABEL,
    },
    {
        "Queue",
        Queue_Tests,
        "Queue built on top of the Span storage architecture.",
        FEATURE_COMPLETE,
    },
    {
        "QueueNext",
        QueueNext_Tests,
        "Queue iteration behaviour, built on top of the Span storage architecture.",
        FEATURE_COMPLETE,
    },
    {
        "QueueMixed",
        QueueMixed_Tests,
        "Queue iteration with a mix of items missing, built on top of the Span storage architecture.",
        FEATURE_COMPLETE,
    },
#ifdef CYCLE_SERVE
    {
        "CycleServe IO Server",
        NULL,
        NULL,
        SECTION_LABEL,
    },
    {
        "Serve",
        Serve_Tests,
        "Basic CycleServe network server initialization tests.",
        FEATURE_COMPLETE,
    },
    {
        "ServeHandle",
        ServeHandle_Tests,
        "CycleServe network server request handling tests.",
        FEATURE_COMPLETE,
    },
    {
        "ServeChunked",
        ServeChunked_Tests,
        "Chunked request (partial HTTP request sent in different requests) for CycleServe network server tests.",
        FEATURE_COMPLETE,
    },
    {
        "ServeMultiple",
        ServeMultiple_Tests,
        "Concurrent request testing for CycleServe network server.",
        FEATURE_COMPLETE,
    },
    {
        "ServeCookieSession",
        Blank_Tests,
        "Tests to use Roebling to open a session based on an HTTP Cookie Header using IoCtx and Crypto.",
        PREVIOUSLY_WORKING,
    },
    {
        "ServeScheduled",
        Blank_Tests,
        "Regularly scheduled activities in CycleServe using the Queue object.",
        NOT_STARTED,
    },
#endif
    {
        "Persistance and Storage",
        NULL,
        NULL,
        SECTION_LABEL,
    },
    {
        "IoCtx",
        IoCtx_Tests,
        "File and storage persistance context tests.",
        FEATURE_COMPLETE,
    },
    {
        "MemLocal - To/From",
        MemLocal_ToFromTests,
        "Calling To and From persistance functions for MemLocal.",
        FEATURE_COMPLETE,
    },
    {
        "MemLocal",
        MemLocal_Tests,
        "Calling save and restore MemLocal functions.",
        FEATURE_COMPLETE,
    },
    {
        "Programming Language and Tools",
        NULL,
        NULL,
        SECTION_LABEL,
    },
    {
        "NestedD Flat",
        NestedDFlat_Tests,
        "NestedD is a stack based data store for lexical reslution of dataused for template langages templating (such as HTML), Flat scenario.",
        FEATURE_COMPLETE,
    },
    {
        "NestedD With",
        NestedDWith_Tests,
        "NestedD is a stack based data store for lexical reslution of dataused for template langages templating (such as HTML), With scenario.",
        FEATURE_COMPLETE,
    },
    {
        "NestedD For",
        NestedDFor_Tests,
        "NestedD is a stack based data store for lexical reslution of dataused for template langages templating (such as HTML), For scenario.",
        FEATURE_COMPLETE,
    },
    {
        "XML Templating",
        XmlTTemplate_Tests,
        "XML templating for building web applications.",
        FEATURE_COMPLETE,
    },
    {
        "CanekaLang - Roebling Syntax",
        Roebling_SyntaxTests,
        "Tests for the Roebling pattern match portion of Caneka syntax.",
        PARTIAL_FEATURE,
    },
    {
        "Security",
        NULL,
        NULL,
        SECTION_LABEL,
    },
    {
        "TextFilter",
        TextFilter_Tests,
        "Filter any non-visible character below 32 (dec)",
        FEATURE_COMPLETE,
    },
    {
        "Crypto - Strings",
        CryptoStrings_Tests,
        "Generating Sha256 summaries of strings. And random data strings.",
        FEATURE_COMPLETE,
    },
    /*
    {
        "Crypto - KeyTests",
        Crypto_KeyTests,
        "Makein Ecsda keys and reading pem files.",
        PARTIAL_FEATURE,
    },
    */
    {
        "Crypto - Ecdsa",
        Blank_Tests,
        "Ecsda sugnatures for strings.",
        NOT_STARTED,
    },
    {
        "Crypto - Salty",
        Salty_Tests,
        "Bi-directional encryption with a variable sized key.",
        PARTIAL_FEATURE,
    },
    {
        "UserTests - PasswordFlow",
        User_Tests,
        "A Basic user password flow using CycleServe and IoCtx.",
        PARTIAL_FEATURE,
    },
    {
        "Email",
        NULL,
        NULL,
        SECTION_LABEL,
    },
    {
        "Smtp",
        Blank_Tests,
        "SMTP (email) format parsing with the Roebling parser.",
        PREVIOUSLY_WORKING,
    },
    {
        "SmtpSend",
        Blank_Tests,
        "Email send flow tests with CycleServe and Roebling.",
        NOT_STARTED,
    },
    {
        NULL,
        NULL,
        NULL,
        0,
    },
};

TestSet *Tests = _Tests;

/*
static TestSet solo[] = {
    {
        "String - B64",
        StringB64_Tests,
        "Tests base 64 encoded strings",
        FEATURE_COMPLETE,
    },
    {
        "Crypto - KeyTests",
        Crypto_KeyTests,
        "Makein Ecsda keys and reading pem files.",
        PARTIAL_FEATURE,
    },
    {
        NULL,
        NULL,
        NULL,
        0
    },
};

TestSet *Tests = solo;
*/
