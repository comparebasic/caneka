#include <external.h>
#include <caneka.h>

status Persisted_Save(MemCtx *m, MemCtx *keyed, String *path, IoCtx *ioctx);
MemCtx *Persisted_Load(MemCtx *m, String *path, IoCtx *ioctx);
