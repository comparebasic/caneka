#include <external.h>
#include <caneka.h>
#include <signer.h>

void SignerCtx_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    DebugStack_Push("SignerCtx_Print", TYPE_CSTR);
    SignerCtx *ctx = as(a, TYPE_SIGNER_CTX);
    printf("\x1b[%dmSignerCtx(%s)<", color, State_ToChars(ctx->type.state));
    DPrint((Abstract *)ctx->filePath, color, " filePath: ");
    DPrint((Abstract *)ctx->identTbl, color, " identTbl: ");
    DPrint((Abstract *)ctx->content, color, " content: ");
    DPrint((Abstract *)ctx->rbl, color, " rbl: ");
    printf("\x1b[%dm>\x1b[0m", color);
    DebugStack_Pop();
    return;
}
