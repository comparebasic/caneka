#include <external.h>
#include <caneka.h>

static boolean _init = FALSE;

static status Tls_Open(Capsule *cap){
    MemCh *m = cap->m;
    i32 ret = 0;

    TlsInfo *info = (TlsInfo *)cap->source;
    if(info->tls == NULL){
        info->tls = SSL_new(info->ctx->tlsCtx);
        if(info->tls == NULL){
            ERR_print_errors_fp(stderr);
            fflush(stdout);
            Error(m, FUNCNAME, FILENAME, LINENUMBER,
                "Error making tls tunnel", NULL);
            cap->type.state |= ERROR;
            close(info->fd);
            return cap->type.state;
        }

        ret = SSL_set_fd(info->tls, info->fd);
        if(ret <= 0){
            Error(m, FUNCNAME, FILENAME, LINENUMBER, "Unable to set fd for SSL", NULL);
            cap->type.state |= ERROR;
            return cap->type.state;
        }
    }

    ret = SSL_accept(info->tls);
    if(ret == 1){
        cap->type.state |= SUCCESS;
    }if(ret <= 0){
        i32 err = SSL_get_error(info->tls, ret);
        if(err == SSL_ERROR_WANT_READ || err == SSL_ERROR_WANT_WRITE){
            ERR_print_errors_fp(stdout);
            cap->type.state |= MORE;
        }else{
            ERR_print_errors_fp(stdout);
            Error(m, FUNCNAME, FILENAME, LINENUMBER, "Unable to connect using TLS", NULL);
            cap->type.state |= ERROR;
            return cap->type.state;
        }
    }

    return cap->type.state;
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
    TlsInfo *info = (TlsInfo *)cap->source;

    Str s = {
        .type = {TYPE_STR, STRING_CONST},
        .alloc=STR_DEFAULT,
        .length=0,
        .bytes = NULL
    };
    byte _bytes[STR_DEFAULT];
    s.bytes = _bytes;

    i32 total = 0;
    while((cap->out->type.state & (END|ERROR)) == 0){
        Buff_GetStr(cap->out, &s);
        SSL_write(info->tls, s.bytes, s.length);
        total += s.length;
        s.length = 0;
    }
    return total > 0 ? SUCCESS : NOOP;
}

static status Tls_ReadTo(Capsule *cap){
    Str s = {
        .type = {TYPE_STR, STRING_CONST},
        .alloc=STR_DEFAULT,
        .length=STR_DEFAULT,
        .bytes = NULL
    };
    byte _bytes[STR_DEFAULT];
    s.bytes = _bytes;

    TlsInfo *info = (TlsInfo *)cap->source;
    i32 read = SSL_read(info->tls, s.bytes, s.length);
    printf("Read %d\n", read);
    if(read > 0){
        s.length = (i16) read;
        Buff_Add(cap->in, &s);
        return SUCCESS;
    }
    return NOOP;
}

TlsCtx *TlsCtx_Make(MemCh *m, StrVec *cert, StrVec *key){
    TlsCtx *ctx = MemCh_AllocOf(m, sizeof(TlsCtx), TYPE_TLS_CTX);
    ctx->type.of = TYPE_TLS_CTX;

    const SSL_METHOD *method = TLS_server_method();
    SSL_CTX *tlsCtx = SSL_CTX_new(method);
    if (!tlsCtx) {
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "Unable to create OpenSSL SSL Ctx", NULL); 
        ctx->type.state |= ERROR;
        ERR_print_errors_fp(stderr);
        return NULL;
    }
    i32 ret = 0;
    ret = SSL_CTX_use_certificate_file(tlsCtx, Ifc(m, cert, TYPE_CSTR), SSL_FILETYPE_PEM);
    if (ret <= 0) {
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "Unable to open cert for OpenSSL SSL Ctx", NULL); 
        ERR_print_errors_fp(stderr);
        ctx->type.state |= ERROR;
        SSL_CTX_free(tlsCtx);
        return NULL;
    }else{
        void *ar[] = {
            cert,
            NULL
        };
        Out("^y.Cert loaded $^0\n", ar);
    }

    ret = SSL_CTX_use_PrivateKey_file(tlsCtx, Ifc(m, key, TYPE_CSTR), SSL_FILETYPE_PEM);
    if (ret <= 0) {
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "Unable to open key for OpenSSL SSL Ctx", NULL); 
        ctx->type.state |= ERROR;
        SSL_CTX_free(tlsCtx);
        return NULL;
    }else{
        void *ar[] = {
            key,
            NULL
        };
        Out("^y.Key loaded $^0\n", ar);
    }


    if(!SSL_CTX_check_private_key(tlsCtx)){
        void *ar[] = {
            key, cert, NULL
        };
        Error(m, FUNCNAME, FILENAME, LINENUMBER, "Error cert and key dont match: $ -> $", ar);
    }else{
        void *ar[] = {
            key,
            cert,
            NULL
        };
        Out("^y.Cert and Key match $ -> $^0\n", ar);
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
    if(!_init){
        _init = TRUE;
        SSL_library_init();
        OpenSSL_add_all_algorithms();
        SSL_load_error_strings();
        CapsuleDef *def = CapsuleDef_Make(m, 
            (Func)Tls_Open, (Func)Tls_Close, (Func)Tls_ReadTo, (Func)Tls_WriteTo);
        Lookup_Add(m, CapsuleDefLookup, TYPE_TLS_CAPSULE, def);
        Tls_ToSInit(m, ToStreamLookup);
    }
}
