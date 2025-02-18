#include <external.h>
#include <caneka.h>
#include <signer.h>

static status SignerCtx_streamIdent(MemCtx *m, String *s, Abstract *source){
    Roebling *rbl = as(source, TYPE_ROEBLING);
    Roebling_Add(rbl, s);
    return Roebling_Run(rbl);
}

status SignerCtx_DigestIdent(SignerCtx *ctx){
    status r = READY;
    File file;
    File_Init(&file, File_GetCwdPath(ctx->m, ctx->configPath), NULL, NULL);
    file.abs = file.path;
    r |= File_Stream(ctx->m, &file, NULL, SignerCtx_streamIdent, (Abstract *)ctx->rbl);
    return r;
}

status SignerCtx_Sign(SignerCtx *ctx){
    String *key = (String *)Table_Get(ctx->identTbl, 
        (Abstract *)String_Make(ctx->m, bytes("key")));
    if(key != NULL){
        ProcDets pd;
        ProcDets_Init(&pd);
        Span *cmd = Span_Make(ctx->m, TYPE_SPAN);
        Span_Add(cmd, (Abstract *)String_Make(ctx->m, bytes("openssl")));
        Span_Add(cmd, (Abstract *)String_Make(ctx->m, bytes("dgst")));
        Span_Add(cmd, (Abstract *)String_Make(ctx->m, bytes("-sha256")));
        Span_Add(cmd, (Abstract *)String_Make(ctx->m, bytes("-sign")));
        Span_Add(cmd, (Abstract *)key);
        Span_Add(cmd, (Abstract *)ctx->filePath);
        SubProcess(ctx->m, cmd, &pd);
        if(pd.type.state & SUCCESS){
            String *keyId = String_Make(ctx->m, bytes("key-id"));
            String *value = (String *)Table_Get(ctx->identTbl, (Abstract *)keyId);
            if(value != NULL){
                String *k = String_Make(ctx->m, bytes("signature-key-id"));
                Table_Set(ctx->summaryTbl, (Abstract *)k, (Abstract *)value);
            }
            String *signature = String_FromFd(ctx->m, pd.outFd);
            Table_Set(ctx->summaryTbl, 
                (Abstract *)String_Make(ctx->m, bytes("signature-type")), 
                (Abstract *)String_Make(ctx->m, bytes("private-key")));
            Table_Set(ctx->summaryTbl, 
                (Abstract *)String_Make(ctx->m, bytes("signature")), 
                (Abstract *)String_ToB64(ctx->m, signature));
            return SUCCESS;
        }
        return ERROR;
    }else{
        return NOOP;
    }
}

status SignerCtx_SetupSummary(SignerCtx *ctx){
    status r = READY;

    String *key = String_Make(ctx->m, bytes("end"));
    String *value = (String *)Table_Get(ctx->headerTbl, 
        (Abstract *)key);
    if(value != NULL){
        Table_Set(ctx->summaryTbl, (Abstract *)value, (Abstract *)value);
    }

    key = String_Make(ctx->m, bytes("digest"));
    String *shaDigest = String_Sha256(ctx->m, ctx->content);
    String *shaHex = String_ToHex(ctx->m, shaDigest);
    Table_Set(ctx->summaryTbl, (Abstract *)key, (Abstract *)shaHex);

    key = String_Make(ctx->m, bytes("digest-type"));
    value = String_Make(ctx->m, bytes("sha256"));
    if(value != NULL){
        Table_Set(ctx->summaryTbl, (Abstract *)key, (Abstract *)value);
    }
    key = String_Make(ctx->m, bytes("end"));
    value = (String *)Table_Get(ctx->headerTbl, (Abstract *)key);
    if(value != NULL){
        r |= Table_Set(ctx->summaryTbl, (Abstract *)value, (Abstract *)value);
    }
    key = String_Make(ctx->m, bytes("party"));
    value = (String *)Table_Get(ctx->identTbl, (Abstract *)key);
    if(value != NULL){
        r |= Table_Set(ctx->summaryTbl, (Abstract *)key, (Abstract *)value);
    }
    key = String_Make(ctx->m, bytes("role"));
    value = (String *)Table_Get(ctx->identTbl, (Abstract *)key);
    if(value != NULL){
        r |= Table_Set(ctx->summaryTbl, (Abstract *)key, (Abstract *)value);
    }
    return r;
}

status SignerCtx_SummaryOut(SignerCtx *ctx, OutFunc out){
    status r = READY;
    String *key = String_Make(ctx->m, bytes("end"));
    r |= Table_Set(ctx->summaryTbl, (Abstract *)key, (Abstract *)key);
    r |= Kve_OutFromTable(ctx->m, ctx->summaryTbl, NULL, out);
    out(ctx->m, String_Make(ctx->m, bytes("\n")), NULL);
    return r;
}

status SignerCtx_HeaderOut(SignerCtx *ctx, OutFunc out){
    status r = READY;
    i64 length = String_Length(ctx->content);
    Table_Set(ctx->headerTbl, 
        (Abstract *)String_Make(ctx->m, bytes("length")), 
        (Abstract *)I64_Wrapped(ctx->m, length));
    
    String *end = String_Make(ctx->m, bytes("end"));
    String *endName = end;
    String *name = (String *)Table_Get(ctx->headerTbl,
        (Abstract *)String_Make(ctx->m, bytes("kve")));
    if(name == NULL){
        Fatal("Name is required for the header of the document", 0);
        r |= ERROR;
        return r;
    }
    endName = String_Init(ctx->m, STRING_EXTEND);
    char *cstr = "end-";
    String_AddBytes(ctx->m, endName, bytes(cstr), strlen(cstr));
    String_Add(ctx->m, endName, name);
    Table_Set(ctx->headerTbl, (Abstract *)end, (Abstract *)endName);

    r |= Kve_OutFromTable(ctx->m, ctx->headerTbl, String_Make(ctx->m, bytes("kve")), out);
    out(ctx->m, String_Make(ctx->m, bytes("\n")), NULL);
    return r;
}

SignerCtx *SignerCtx_Make(MemCtx *m){
    SignerCtx *ctx = MemCtx_Alloc(m, sizeof(SignerCtx));
    ctx->type.of = TYPE_SIGNER_CTX;
    ctx->m = m;
    ctx->identTbl = (Span *)OrderedTable_Make(m);
    ctx->headerTbl = (Span *)OrderedTable_Make(m);
    ctx->summaryTbl = (Span *)OrderedTable_Make(m);
    return ctx;
}
