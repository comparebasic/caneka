#define USER_SALT_LENGTH 113
Span *User_Open(MemCtx *m, IoCtx *userCtx, String *id, String *secret, Access *ac, Span *data);
