status Guard_Reset(i16 *g);
status Guard_Incr(struct mem_ctx *m, i16 *g, i16 max, char *func, char *file, int line);
boolean Guard(i16 *g, i16 max, char *func, char *file, int line);
