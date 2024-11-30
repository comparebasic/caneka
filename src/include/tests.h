#include <mock_109strings.h>
extern char *longCstr;

extern TestSet *Tests;

status Blank_Tests(MemCtx *gm);

status Core_Tests(MemCtx *gm);
status String_Tests(MemCtx *gm);
status Span_Tests(MemCtx *gm);
status SpanInline_Tests(MemCtx *gm);
status Hash_Tests(MemCtx *gm);
status Roebling_Tests(MemCtx *gm);
status RoeblingRun_Tests(MemCtx *gm);
status RoeblingMark_Tests(MemCtx *gm);
status Table_Tests(MemCtx *gm);
status Xml_Tests(MemCtx *gm);
status XmlNested_Tests(MemCtx *gm);
status Match_Tests(MemCtx *gm);
status SpanSetup_Tests(MemCtx *gm);
status TableResize_Tests(MemCtx *gm);
status TablePreKey_Tests(MemCtx *gm);
status MatchElastic_Tests(MemCtx *gm);
status XmlParser_Tests(MemCtx *gm);
status Http_Tests(MemCtx *gm);
status SpanClone_Tests(MemCtx *gm);
status RoeblingStartStop_Tests(MemCtx *gm);
status Queue_Tests(MemCtx *gm);
status QueueNext_Tests(MemCtx *gm);
status QueueMixed_Tests(MemCtx *gm);
status Serve_Tests(MemCtx *gm);
status ServeHandle_Tests(MemCtx *gm);
status ServeChunked_Tests(MemCtx *gm);
status ServeMultiple_Tests(MemCtx *gm);
status IoCtx_Tests(MemCtx *gm);
status Oset_Tests(MemCtx *gm);
status MemLocal_Tests(MemCtx *gm);
status NestedDFlat_Tests(MemCtx *gm);
status NestedDWith_Tests(MemCtx *gm);
status NestedDFor_Tests(MemCtx *gm);
status XmlStringType_Tests(MemCtx *gm);
status XmlTTemplate_Tests(MemCtx *gm);
status TextFilter_Tests(MemCtx *gm);
status Sha256_Tests(MemCtx *gm);
status Ecdsa_Tests(MemCtx *gm);
