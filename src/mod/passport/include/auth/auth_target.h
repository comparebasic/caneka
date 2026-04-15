enum auth_flags {
    AUTH_I = 1 << 8,
    AUTH_U = 1 << 9,
    AUTH_IP4 = 1 << 10,
    AUTH_IP6 = 1 << 11,
    AUTH_SIG = 1 << 12,
    AUTH_TEXT = 1 << 13,
};

typedef struct auth_target {
    Type type;
    union {
        i32 i;
        util u;
        util u128[2];
        StrVec *text;
    } bin;
    Span *groups;
} AuthTarget;

AuthTarget *AuthTarget_Make(MemCh *m, word flags);

status AuthFuncI(MemCh *m, AuthTarget *target, digest *hash);
status AuthFuncU(MemCh *m, AuthTarget *target, digest *hash);
status AuthFuncVec(MemCh *m, AuthTarget *target, digest *hash);
status AuthFuncIp6(MemCh *m, AuthTarget *target, digest *hash);

status AuthTarget_Init(MemCh *m, digest *hash);
