typedef struct strvec_entry {
    Type type;
    int length;
    byte *content;
    struct strvec_entry *jump;
} StrVecEntry;

typedef struct strvec_hdr {
    Type type;
    i32 count;
    i64 total;
    struct strvec_entry *last;
} StrVec;

status StrVec_ToFd(StrVec *vh, int fd);
String *StrVec_ToStr(MemCtx *m, StrVec *vh);
status StrVec_Add(MemCtx *m, StrVec *vh, byte *ptr, int length);
StrVec *StrVec_Make(MemCtx *m, byte *ptr, int length);
status StrVecEntry_Set(StrVecEntry *ve, byte *ptr, int length);
