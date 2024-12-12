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

boolean HasAccess(String *perm, Access *access);
Access *Access_Make(MemCtx *m, String *owner, Span *groups);
