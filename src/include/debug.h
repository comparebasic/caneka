/* 
Debug
Configuration file for showing debugging information, the debug flags are located in core/debug.c

related: core/debug.c
*/
#define COLOR_RED 31
#define COLOR_GREEN 32
#define COLOR_YELLOW 33
#define COLOR_BLUE 34
#define COLOR_PURPLE 35
#define COLOR_CYAN 36
#define COLOR_DARK 37

extern MemCtx *DebugM;

typedef void (*DebugPrintFunc)(void *t, cls type, char *msg, int color, boolean extended);

extern Chain *DebugPrintChain;
status Debug_Init(MemCtx *m);
void Debug_Print(void *t, cls type, char *msg, int color, boolean extended);
void Bits_Print(byte *bt, int length, char *msg, int color, boolean extended);
void indent_Print(int indent);
void Match_midDebug(char type, word c, PatCharDef *def, Match *mt, boolean matched);

extern char *rbl_debug_cstr[];

extern int DEBUG_MATCH;
extern int DEBUG_PATMATCH;
extern int DEBUG_MATCH_COMPLETE;
extern int DEBUG_ALLOC;
extern int DEBUG_SCURSOR;
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
extern int DEBUG_HTTP;
extern int DEBUG_SERVE;
extern int DEBUG_SERVE_ACCEPT;
extern int DEBUG_SERVE_ROUNDS;
extern int DEBUG_REQ;
extern int DEBUG_EXAMPLE_HANDLERS;
extern int DEBUG_QUEUE;


void SpanQuery_Print(Abstract *a, cls type, char *msg, int color, boolean extended);
void SpanState_Print(Abstract *a, cls type, char *msg, int color, boolean extended);
void SpanDef_Print(SpanDef *def);
