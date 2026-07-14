#include <external.h>
#include <caneka.h>

static status Tls_Open(Capsule *cap){
    MemCh *m = cap->m;
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

static status Tls_Close(Capsule *cap){
    MemCh *m = cap->m;
    TlsInfo *info = (TlsInfo *)cap->source;
    SSL_shutdown(info->tls);
    SSL_free(info->tls);
    close(info->fd);
    return ZERO;
}

static status Tls_WriteTo(Capsule *cap){
    return ZERO;
}

static status Tls_ReadTo(Capsule *cap){
    return ZERO;
}

TlsCtx *TlsCtx_Make(MemCh *m, StrVec *cert, StrVec *key){
    TlsCtx *ctx = MemCh_AllocOf(m, sizeof(TlsCtx), TYPE_TLS_CTX);
    ctx->type.of = TYPE_TLS_CTX;

    const SSL_METHOD *method = TLS_server_method();
    SSL_CTX *tlsCtx = SSL_CTX_new(method);
    if (!tlsCtx) {
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "Unable to create OpenSSL SSL Ctx", NULL); 
        ERR_print_errors_fp(stderr);
        return NULL;
    }
    if (SSL_CTX_use_certificate_file(tlsCtx, Ifc(m, cert, TYPE_CSTR), SSL_FILETYPE_PEM) <= 0) {
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "Unable to open cert for OpenSSL SSL Ctx", NULL); 
        ERR_print_errors_fp(stderr);
        SSL_CTX_free(tlsCtx);
        return NULL;
    }
    if (SSL_CTX_use_PrivateKey_file(tlsCtx,
            Ifc(m, key, TYPE_CSTR), SSL_FILETYPE_PEM) <= 0) {
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "Unable to open key for OpenSSL SSL Ctx", NULL); 
        SSL_CTX_free(tlsCtx);
        return NULL;
    }

    SSL_CTX_set_min_proto_version(tlsCtx, TLS1_2_VERSION);
    ctx->tlsCtx = tlsCtx;

    return ctx;
}

void TlsCtx_Destroy(MemCh *m, TlsCtx *ctx){
    SSL_CTX_free(ctx->tlsCtx);
}

TlsInfo *TlsInfo_Make(MemCh *m, i32 fd, TlsCtx *ctx){
    TlsInfo *info = MemCh_AllocOf(m, sizeof(TlsInfo), TYPE_TLS_INFO);
    info->type.of = TYPE_TLS_INFO;
    info->fd = fd;
    info->ctx = ctx;
    return info;
}

void Tls_Init(MemCh *m){
    CapsuleDef *def = CapsuleDef_Make(m, 
        (Func)Tls_Open, (Func)Tls_Close, (Func)Tls_ReadTo, (Func)Tls_WriteTo);
    Lookup_Add(m, CapsuleDefLookup, TYPE_TLS_CAPSULE, def);
}
