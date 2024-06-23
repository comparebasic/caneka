#define COLOR_RED 31
#define COLOR_GREEN 32
#define COLOR_YELLOW 33
#define COLOR_BLUE 34
#define COLOR_PURPLE 35
#define COLOR_CYAN 36
#define COLOR_DARK 37

void Debug_Print(void *t, cls type, char *msg, int color, boolean extended);

extern int DEBUG_MATCH;
extern int DEBUG_PATMATCH;
