#define isDebug(x) (((x)->type.state & DEBUG) != 0)

typedef i64 (*DebugPrintFunc)(MemCh *m, StrVec *v, i32 fd, Abstract *a, cls type, boolean extended);

extern struct lookup *DebugPrintChain;
extern char *TypeStrings[];
extern boolean SHOW_SERVE_TESTS;
extern MemCh *_debugM;

status Debug_Init(MemCh *m);
void Bits_Print(byte *bt, int length, char *msg, int color, boolean extended);
void indent_Print(int indent);
void Match_midDebug(char type, word c, struct patchardef *def, struct match *mt, boolean matched, boolean extended);

extern char *rbl_debug_cstr[];

const char *Type_ToChars(cls type);
Str *State_ToStr(MemCh *m, status state);
i64 Str_Debug(MemCh *m, StrVec *v, i32 fd, void *t, cls type, boolean extended);
