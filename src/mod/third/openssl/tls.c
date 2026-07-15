#include <external.h>
#include <caneka.h>

static boolean _init = FALSE;

static status Tls_Open(Capsule *cap){
    printf("Tls_Open...\n");
    MemCh *m = cap->m;
    i32 ret = 0;

    TlsInfo *info = (TlsInfo *)cap->source;
    printf("Info %p\n", info);
    fflush(stdout);
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
            printf("SSL fd not set! %s", ERR_error_string(SSL_get_error(info->tls, ret), NULL));
            Error(m, FUNCNAME, FILENAME, LINENUMBER, "Unable to set fd for SSL", NULL);
            cap->type.state |= ERROR;
            return cap->type.state;
        }
        printf("Re-making Tls %p\n", info->tls);
        fflush(stdout);
    }else{
        printf("Not re-making Tls %p\n", info->tls);
        fflush(stdout);
    }

    printf("before\n");
    fflush(stdout);
    ret = SSL_accept(info->tls);
    printf("after\n");
    fflush(stdout);
    if(ret == 1){
        cap->type.state |= SUCCESS;
        printf("SSL Working!!!\n");
        fflush(stdout);
    }if(ret <= 0){
        printf("Err ret %d\n", ret);
        fflush(stdout);
        i32 err = SSL_get_error(info->tls, ret);
        if(err == SSL_ERROR_WANT_READ || err == SSL_ERROR_WANT_WRITE){
            ERR_print_errors_fp(stdout);
            printf("Not Yet ret %d %s\n", err, ERR_error_string(err, NULL));
            fflush(stdout);
            cap->type.state |= MORE;
        }else{
            ERR_print_errors_fp(stdout);
            printf("Other Err ret %d %s\n", err, ERR_error_string(err, NULL));
            fflush(stdout);
            Error(m, FUNCNAME, FILENAME, LINENUMBER, "Unable to connect using TLS", NULL);
            cap->type.state |= ERROR;
            return cap->type.state;
        }
    }

    printf("SSL Opened!\n");
    fflush(stdout);
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
    return ZERO;
}

static status Tls_ReadTo(Capsule *cap){
    Str *s = Str_Make(m, STR_DEFAULT);
    i32 read = SSL_read(cap->info->tls, s->bytes, s->length);
    if(read > 0){
        Buff_Add(cap->in, s);
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
    printf("Cert path %s\n", Ifc(m, cert, TYPE_CSTR));
    fflush(stdout);
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
    printf("SSLCtx %p\n", ctx->tlsCtx);

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
