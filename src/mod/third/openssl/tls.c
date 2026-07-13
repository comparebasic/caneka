#include <external.h>
#include <caneka.h>

static status Tls_Open(MemCh *m, Capsule *cap, TlsInfo *info){
    TlsInfo *info = (TlsInfo *)cap->source;
    info->tls = SSL_new(info->ctx->tlsCtx);
    SSL_set_fd(info->tls, info->fd);
    if(SSL_connect(info->tls) <= 0){
        Error(m, FUNCNAME, FILENAME, LINENUMBER, "Unable to connect using TLS", NULL);
        cap->type.state |= ERROR;
        return cap->type.state;
    }
    return ZERO;
}

static status Tls_Close(MemCh *m, Capsule *cap, TlsInfo *info){
    TlsInfo *info = (TlsInfo *)cap->source;
    SSL_shutdown(info->tls);
    SSL_free(info->tls);
    close(info->fd);
}

static status Tls_WriteTo(MemCh *m, Capsule *cap, TlsInfo *info){
    return ZERO;
}

static status Tls_ReadTo(MemCh *m, Capsule *cap, TlsInfo *info){
    return ZERO;
}

TlsCtx *TlsCtx_Make(MemCh *m, StrVec *cert, StrVec *key){
    TlsCtx *ctx = MemCh_AllocOf(m, sizeof(TlsCtx), TYPE_TLS_CTX);
    ctx->type.of = TYPE_TLS_CTX;
    SSL_CTX *ctx->tlsCtx = SSL_CTX_new(TLS_client_method());
    return ctx;
}

void TlsCtx_Destroy(MemCh *m, TlsCtx *ctx){
    SSL_CTX_free(ctx);
}

TlsInfo *TlsInfo_Make(MemCh *m, i32 fd, TlsCtx *ctx){
    TlsInfo *info = MemCh_AllocOf(m, sizeof(TlsInfo), TYPE_TLS_INFO);
    info->type.of = TYPE_TLS_INFO;
    info->fd = fd;
    info->ctx = ctx;
    return info;
}

void Tls_Init(MemCh *m){
    CapsuleDef *def = Capsule_Make(m, Tls_Open, Tls_Close, Tls_ReadTo, Tls_WriteTo);
    Lookup_Add(m, CapsuleDefLookup, TYPE_TLS_CAPSULE, def);
}
