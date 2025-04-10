status Guard_Init(Guard *g, int max);
status Guard_Setup(MemCh *m, Guard *g, int max, byte *msg);
status Guard_Reset(Guard *g);
status _Guard_Incr(Guard *g, char *func, char *file, int line);
