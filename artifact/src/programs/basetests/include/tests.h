#include "mock_109strings.h"

extern char *longCstr;

extern TestSet *Tests;

/* tests */
status MemCh_Tests(MemCh *gm);
status Span_Tests(MemCh *gm);
status Core_Tests(MemCh *gm);
status Str_Tests(MemCh *gm);
status StrB64_Tests(MemCh *gm);
status Str_EndMatchTests(MemCh *gm);
status StrVec_Tests(MemCh *gm);
status SpanInline_Tests(MemCh *gm);
status Hash_Tests(MemCh *gm);
status Table_Tests(MemCh *gm);
status TableResize_Tests(MemCh *gm);
status TablePreKey_Tests(MemCh *gm);
status SpanClone_Tests(MemCh *gm);
