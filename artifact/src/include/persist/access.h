enum access_flags {
    ACCESS_CREATE = 1 << 8,
    ACCESS_KEY = 1 << 9,
    ACCESS_SPAWN = 1 << 10,
    ACCESS_READ = 1 << 11,
    ACCESS_ADD = 1 << 12,
    ACCESS_MODIFY = 1 << 13,
    ACCESS_DELETE = 1 << 14,
};

typedef struct signed_content {
    Type type;
    String *content;
    String *keyId;
    String *signature;
} SignedContent;

typedef struct permission {
    Type type;
    Abstract *a;
} Permission;

typedef struct access {
    Type type;
    String *owner;
    Span *groups;
    SignedContent *cert;
} Access;

boolean HasAccess(Access *access, Abstract *a);
Access *Access_Make(MemCtx *m, String *owner, Span *groups);
Permission *Access_MakePermission(MemCtx *m, word flags, Abstract *a);

#define Access_SetFl(access, fl) \
    ((access)->type.state = (((access)->type.state & NORMAL_FLAGS) | fl))
