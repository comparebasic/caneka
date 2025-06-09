#define SPOOL_STRING_COUNT 4 * STRING_CHUNK_SIZE
File *Spool_Make(MemCh *m, Str *path, Access *access, IoCtx *ctx);
File *Spool_Init(File *file, Str *path, Access *access, IoCtx *ctx);
status Spool_Add(MemCh *m, Str *s, Abstract *a);
status Spool_Trunc(File *file);
