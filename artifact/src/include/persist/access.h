enum access_flags {
    ACCESS_CREATE = 1 << 8,
    ACCESS_KEY = 1 << 9,
    ACCESS_SPAWN = 1 << 10,
    ACCESS_READ = 1 << 11,
    ACCESS_ADD = 1 << 12,
    ACCESS_WILD = 1 << 13,
    ACCESS_DELETE = 1 << 14,
    ACCESS_GRANT = 1 << 15,
};

typedef struct signed_content {
    Type type;
    Str *content;
    Str *keyId;
    Str *signature;
} SignedContent;

typedef struct access {
    Type type;
    Str *owner;
    Span *groups; /* acccess objects */
    SignedContent *cert;
} Access;

status Access_Init(MemCtx *m);
Access *Access_Make(MemCtx *m, Str *owner, Span *groups);
Str *GetAccess(Access *access, Str *s);
Str *GetGroupAccess(Access *access, Str *s);
status Access_Grant(MemCtx *m, Access *grantee, word fl, Str *key, Abstract *value, Access *access);

#define Access_SetFl(access, fl) \
    ((access)->type.state = (((access)->type.state & NORMAL_FLAGS) | fl))
