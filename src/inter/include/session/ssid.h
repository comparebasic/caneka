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

StrVec *Ssid_From(SsidCtx *ctx, StrVec *ua, microTime time);
status Ssid_Open(SsidCtx *ctx, StrVec *ssid, StrVec *ua);
StrVec *Ssid_Update(SsidCtx *ctx, StrVec *ssid, StrVec *ua, microTime time);
StrVec *Ssid_Start(SsidCtx *ctx, StrVec *ua, microTime time);
status Ssid_Set(SsidCtx *ctx, StrVec *ssid, StrVec *key, void *value);
void *Ssid_Get(SsidCtx *ctx, StrVec *ssid, StrVec *key);
status Ssid_UnSet(SsidCtx *ctx, StrVec *ssid, StrVec *key);
status Ssid_Close(SsidCtx *ctx, StrVec *ssid, StrVec *ua);
SsidCtx *SsidCtx_Make(MemCh *m, Str *path);
