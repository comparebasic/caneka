#define isDebug(x) (((x)->type.state & DEBUG) != 0)

typedef i64 (*DebugPrintFunc)(MemCtx *m, StrVec *v, Abstract *a, cls type, boolean extended);

extern struct lookup *DebugPrintChain;
extern char *TypeStrings[];
extern boolean SHOW_SERVE_TESTS;
extern MemCtx *_debugM;

status Debug_Init(MemCtx *m);
void DPrint(Abstract *a, int color, char *msg, ...);
void Debug_Print(void *t, cls type, char *msg, int color, boolean extended);
void Bits_Print(byte *bt, int length, char *msg, int color, boolean extended);
void Flag16_ToString(word flag, int color, boolean extended);
void indent_Print(int indent);
void Match_midDebug(char type, word c, struct patchardef *def, struct match *mt, boolean matched, boolean extended);

extern char *rbl_debug_cstr[];

i64 Str_Debug(MemCtx *m, StrVec *v, void *t, cls type, boolean extended);
struct string *State_ToString(struct mem_ctx *m, status state);
char *Class_ToString(cls type);
char *State_ToChars(status state);
char *State_ToChars(status state);
