#define USER_SALT_LENGTH 86
Span *User_Open(MemCh *m, IoCtx *userCtx, Str *id, Str *secret, Access *ac, Span *data);
status User_Delete(MemCh *m, IoCtx *userCtx, Str *id, Access *ac);
