#define IS_FILE 8
#define IS_DIR 4

status Dir_Climb(MemCtx *m, String *path, DoFunc dir, DblFunc file);
