struct signer_ctx;
typedef status (*charSetterFunc)(struct signer_ctx *ctx, char *arg, char *value);
typedef struct si_args  {
    char *arg;
    charSetterFunc func;
} SiArgs;

enum signer_types {
    _TYPE_SIGNER_START = _TYPE_APPS_END,
    TYPE_SIGNER_CTX,
};

typedef struct signer_ctx {
    Type type;
    MemCtx *m;
    String *filePath;
    String *configPath;
    Span *identTbl;
    Span *outputTbl;
    String *content;
    Roebling *rbl;
} SignerCtx;

status Signer_SetArgs(SignerCtx *ctx, int argc, char *argv[]);
void SignerCtx_Print(Abstract *a, cls type, char *msg, int color, boolean extended);
status Signer_Init(MemCtx *m);
SignerCtx *SignerCtx_Make(MemCtx *m);
status SignerCtx_DigestIdent(SignerCtx *ctx);
