void StrSnipString_Print(Abstract *a, cls type, char *msg, int color, boolean extended);
void String_Print(Abstract *a, cls type, char *msg, int color, boolean extended);
void StringFixed_Print(Abstract *a, cls type, char *msg, int color, boolean extended);
void StringFull_Print(Abstract *a, cls type, char *msg, int color, boolean extended);
void Cstr_Print(Abstract *a, cls type, char *msg, int color, boolean extended);
void Cursor_Print(Abstract *a, cls type, char *msg, int color, boolean extended);
status StringDebug_Init(MemCtx *m, struct lookup *lk);
