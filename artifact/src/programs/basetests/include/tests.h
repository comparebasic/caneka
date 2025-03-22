#include "mock_109strings.h"

extern char *longCstr;

extern TestSet *Tests;

/* tests */
status MemCtx_Tests(MemCtx *gm);
status Span_Tests(MemCtx *gm);
status Core_Tests(MemCtx *gm);
status String_Tests(MemCtx *gm);
status StringB64_Tests(MemCtx *gm);
status String_EndMatchTests(MemCtx *gm);
status StrVec_Tests(MemCtx *gm);
status SpanInline_Tests(MemCtx *gm);
status Hash_Tests(MemCtx *gm);
status Table_Tests(MemCtx *gm);
status TableResize_Tests(MemCtx *gm);
status TablePreKey_Tests(MemCtx *gm);
status SpanClone_Tests(MemCtx *gm);
