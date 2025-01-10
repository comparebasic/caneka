typedef struct guard {
    Type type;
    int count;
    int max;
    char *file;
    int line;
    String msg;
} Guard;

status Guard_Setup(MemCtx *m, Guard *g, int max, byte *msg);
status Guard_Init(Guard *g, int max);
status Guard_Reset(Guard *g);
status _Guard_Incr(Guard *g, char *func, char *file, int line);

#ifdef LINUX
#define Guard_Incr(g) _Guard_Incr((g), __func__, __FILE__, __LINE__)
#else
#define Guard_Incr(g) _Guard_Incr((g), "unknown", __FILE__, __LINE__)
#endif
