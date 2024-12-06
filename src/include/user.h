#define USER_SALT_LENGTH 512
Span *User_Open(MemCtx *m, IoCtx *userCtx, String *id, String *pass, Access *ac);
