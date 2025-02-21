enum access_flags {
    ACCESS_CREATE = 1 << 8,
    ACCESS_PATH = 1 << 9,
    ACCESS_SPAWN = 1 << 10,
    ACCESS_GRANT = 1 << 11,
    ACCESS_READ = 1 << 12,
    ACCESS_MODIFY = 1 << 13,
    ACCESS_ADD = 1 << 14,
    ACCESS_DELETE = 1 << 15,
};

typedef struct signed_content {
    Type type;
    String *content;
    String *keyId;
    String *signature;
} SignedContent;

typedef struct access {
    Type type;
    String *owner;
    Span *groups;
    SignedContent *cert;
} Access;

boolean HasAccess(Access *access, Abstract *a);
Access *Access_Make(MemCtx *m, String *owner, Span *groups);
