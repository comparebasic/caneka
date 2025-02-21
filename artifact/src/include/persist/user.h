#define USER_SALT_LENGTH 86
Span *User_Open(MemCtx *m, IoCtx *userCtx, String *id, String *secret, Access *ac, Span *data);
status User_Delete(MemCtx *m, IoCtx *userCtx, String *id, Access *ac);
