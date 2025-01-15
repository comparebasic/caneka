#include "mock_109strings.h"
extern char *longCstr;

extern TestSet *Tests;

status Tests_AddTestKey(MemCtx *m);

/* utils */
int ServeTests_ForkRequest(MemCtx *m, char *msg, ReqTestSpec[]);
status TestChild(int child);
status ServeTests_SpawnRequest(MemCtx *m, char *msg);
IoCtx *IoCtxTests_GetRootCtx(MemCtx *m);

/* tests */
status Blank_Tests(MemCtx *gm);

status Core_Tests(MemCtx *gm);
status String_Tests(MemCtx *gm);
status StringB64_Tests(MemCtx *gm);
status String_EndMatchTests(MemCtx *gm);
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
status MatchKo_Tests(MemCtx *gm);
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
status CryptoStrings_Tests(MemCtx *gm);
status Salty_Tests(MemCtx *gm);
status User_Tests(MemCtx *gm);
status Roebling_SyntaxTests(MemCtx *gm);
status IterStr_Tests(MemCtx *gm);
status ProtoHttp_Tests(MemCtx *gm);
status StrSnip_Tests(MemCtx *gm);
status StrSnipBoundry_Tests(MemCtx *gm);
status MemCtx_Tests(MemCtx *gm);
