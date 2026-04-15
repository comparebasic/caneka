typedef struct ssid_ctx {
    Type type;
    MemCh *m;
    Str *path;
    struct {
        i64 open;
        i64 closed;
        i64 count;
    } metrics;
    Table *seel;
} SsidCtx;

StrVec *Ssid_From(SsidCtx *ctx, StrVec *ua, struct timespec *ts);
StrVec *Ssid_Start(SsidCtx *ctx, StrVec *ua, struct timespec *ts);
Table *Ssid_Open(SsidCtx *ctx, StrVec *ssid, StrVec *ua);
status Ssid_Close(SsidCtx *ctx, StrVec *ssid, StrVec *ua, Table *stashTbl);
StrVec *Ssid_Update(SsidCtx *ctx, StrVec *ssid, StrVec *ua, struct timespec *ts);
status Ssid_Destroy(SsidCtx *ctx, StrVec *ssid, StrVec *ua);
SsidCtx *SsidCtx_Make(MemCh *m, Str *path);
