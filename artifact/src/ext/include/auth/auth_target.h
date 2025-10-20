enum auth_flags {
    AUTH_FD = 1 << 8,
    AUTH_PATH = 1 << 9,
    AUTH_ASYM_KEY, 1 << 10,
    AUTH_SYM_KEY = 1 << 11,
    AUTH_IP4 = 1 << 11,
    AUTH_IP6 = 1 << 12,
    AUTH_SIG = 1 << 13,
    AUTH_HASH = 1 << 13,
    AUTH_TEXT = 1 << 14,
}

typedef struct auth_target {
    Type type;
    union {
        i32 i;
        util u;
        u128;
        StrVec *text;
    } bin.
    Span *groups;
} AuthTarget;

AuthTarget *AuthTarget_Make(MemCh *m);

status AuthFuncI(MemCh *m, AuthTarget *target, digest *hash);
status AuthFuncU(MemCh *m, AuthTarget *target, digest *hash);
status AuthFuncVec(MemCh *m, AuthTarget *target, digest *hash);
status AuthFuncIp6(MemCh *m, AuthTarget *target, digest *hash);

status AuthTarget_Init(MemCh *m, digest *hash);
