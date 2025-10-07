#include "mock_109strings.h"

extern char *longCstr;

extern TestSet *Tests;

status Tests_Init(MemCh *m);

/* tests */
status MemCh_Tests(MemCh *gm);
status Span_Tests(MemCh *gm);
status Core_Tests(MemCh *gm);
status Str_Tests(MemCh *gm);
status StrB64_Tests(MemCh *gm);
status Str_EndMatchTests(MemCh *gm);
status StrVec_Tests(MemCh *gm);
status StrVecSplit_Tests(MemCh *gm);
status SpanInline_Tests(MemCh *gm);
status Hash_Tests(MemCh *gm);
status Clone_Tests(MemCh *gm);
status Table_Tests(MemCh *gm);
status TableResize_Tests(MemCh *gm);
status Stream_Tests(MemCh *gm);
status Cursor_Tests(MemCh *gm);
status PatChar_Tests(MemCh *gm);
status Match_Tests(MemCh *gm);
status MatchElastic_Tests(MemCh *gm);
status MatchKo_Tests(MemCh *gm);
status MatchReplace_Tests(MemCh *gm);
status Snip_Tests(MemCh *gm);
status Roebling_Tests(MemCh *gm);
status RoeblingRun_Tests(MemCh *gm);
status RoeblingMark_Tests(MemCh *gm);
status RoeblingStartStop_Tests(MemCh *gm);
status Mess_Tests(MemCh *gm);
status Relation_Tests(MemCh *gm);
status Iter_Tests(MemCh *gm);
status SnipModify_Tests(MemCh *gm);
status BinPin_Tests(MemCh *gm);
status File_Tests(MemCh *gm);
status FmtHtml_Tests(MemCh *gm);
status DebugStack_Tests(MemCh *gm);
status Object_Tests(MemCh *gm);
status Templ_Tests(MemCh *gm);
status TemplLogic_Tests(MemCh *gm);
status TemplCtx_Tests(MemCh *gm);
status Path_Tests(MemCh *gm);
status WwwNavTempl_Tests(MemCh *gm);
status Persist_Tests(MemCh *gm);
status TableUtilKey_Tests(MemCh *gm);
status TableHKey_Tests(MemCh *gm);
status Time_Tests(MemCh *gm);
status Queue_Tests(MemCh *gm);
status TaskStep_Tests(MemCh *gm);
status ServeTcp_Tests(MemCh *gm);

status TablePreKey_Tests(MemCh *gm);
status SpanClone_Tests(MemCh *gm);
status WwwNav_Tests(MemCh *gm);
