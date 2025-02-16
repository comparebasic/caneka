#define isDebug(x) (((x)->type.state & DEBUG) != 0)

extern struct chain *DebugPrintChain;

extern char *TypeStrings[];

extern int DEBUG_MATCH;
extern int DEBUG_PATMATCH;
extern int DEBUG_MATCH_COMPLETE;
extern int DEBUG_ALLOC;
extern int DEBUG_PARSER;
extern int DEBUG_BOUNDS_CHECK;
extern int DEBUG_ROEBLING;
extern int DEBUG_ROEBLING_MARK;
extern int DEBUG_ROEBLING_COMPLETE;
extern int DEBUG_ROEBLING_CONTENT;
extern int DEBUG_CURSOR;
extern int DEBUG_TABLE;
extern int DEBUG_ROEBLING_CURRENT;
extern int DEBUG_ROEBLING_NAME;
extern int DEBUG_SPAN;
extern int DEBUG_SPAN_GET_SET;
extern int DEBUG_XML;
extern int DEBUG_XML_TEMPLATE;
extern int DEBUG_XML_TEMPLATE_OUT;
extern int DEBUG_XML_TEMPLATE_NESTING;
extern int DEBUG_HTTP;
extern int DEBUG_SERVE;
extern int DEBUG_SERVE_ACCEPT;
extern int DEBUG_SERVE_POLLING;
extern int DEBUG_SERVE_ROUNDS;
extern int DEBUG_REQ;
extern int DEBUG_REQ_RECV;
extern int DEBUG_EXAMPLE_HANDLERS;
extern int DEBUG_QUEUE;
extern int DEBUG_FILE;
extern int DEBUG_OSET;
extern int DEBUG_OSET_DEF;
extern int DEBUG_OSET_COMPLETE;
extern int DEBUG_NESTED;
extern int DEBUG_CASH;
extern int DEBUG_USER;
extern int DEBUG_LANG_TRANSP;
extern int DEBUG_LANG_CNK;
extern int DEBUG_LANG_CNK_RBL;
extern int DEBUG_SUBPROCESS;
extern int DEBUG_LANG_CNK_OUT;
extern int DEBUG_STRSNIP;
extern int DEBUG_CDOC;
extern int DEBUG_MEMLOCAL;
extern int DEBUG_IOCTX;
extern int DEBUG_KVE;

extern boolean SHOW_SERVE_TESTS;
extern MemCtx *DebugM;

typedef void (*DebugPrintFunc)(void *t, cls type, char *msg, int color, boolean extended);

status Debug_Init(MemCtx *m);
void DPrint(Abstract *a, int color, char *msg, ...);
void Debug_Print(void *t, cls type, char *msg, int color, boolean extended);
void Bits_Print(byte *bt, int length, char *msg, int color, boolean extended);
void Flag16_ToString(word flag, int color, boolean extended);
void indent_Print(int indent);
void Match_midDebug(char type, word c, struct patchardef *def, struct match *mt, boolean matched, boolean extended);

extern char *rbl_debug_cstr[];

struct string *State_ToString(struct mem_ctx *m, status state);
char *Class_ToString(cls type);
char *State_ToChars(status state);
void SpanQuery_Print(Abstract *a, cls type, char *msg, int color, boolean extended);
void SpanState_Print(Abstract *a, cls type, char *msg, int color, boolean extended);
void SpanDef_Print(struct span_def *def);
char *Class_ToString(cls type);
char *State_ToChars(status state);
