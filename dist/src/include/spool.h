#define SPOOL_STRING_COUNT 4 * STRING_CHUNK_SIZE
File *Spool_Make(MemCtx *m, String *path, Access *access, IoCtx *ctx);
File *Spool_Init(File *file, String *path, Access *access, IoCtx *ctx);
status Spool_Add(MemCtx *m, String *s, Abstract *a);
status Spool_Trunc(File *file);
