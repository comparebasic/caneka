status DebugStack_Init(MemCh *m);
void _DebugStack_Push(char *cstr, char *fname, void *ref, word typeOf, int line, int pos);
void DebugStack_Pop();
void DebugStack_SetRef(void *v, word typeOf);
int DebugStack_Print();
