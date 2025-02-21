void TableChain_Print(Abstract *a, cls type, char *msg, int color, boolean extended);
void SpanState_Print(Abstract *a, cls type, char *msg, int color, boolean extended);
void SpanQuery_Print(Abstract *a, cls type, char *msg, int color, boolean extended);
void SpanDef_Print(SpanDef *def);
void Span_Print(Abstract *a, cls type, char *msg, int color, boolean extended);
void Lookup_Print(Abstract *a, cls type, char *msg, int color, boolean extended);
status SequenceDebug_Init(MemCtx *m, Lookup *lk);
