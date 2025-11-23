#include <external.h>
#include <caneka.h>

StrVec *Ssid_From(SsidCtx *ctx, StrVec *ua, microTime time){
    MemCh *m = ctx->m;
    quad parity = Parity_FromVec(ua);
    StrVec *v = StrVec_Make(m);

    StrVec_Add(v,
        Str_ToHex(m,
            Str_Ref(m, (byte *)&parity, sizeof(parity), sizeof(parity), STRING_BINARY)));

    StrVec_Add(v, S(m, "-"));
    StrVec_Add(v,
        Str_ToHex(m,
            Str_Ref(m, (byte *)&time, sizeof(time), sizeof(time), STRING_BINARY)));

    StrVec_Add(v, S(m, "-"));
    Str *rand = Str_Make(m, 8);
    Buff_GetStr(RandStream, rand);
    StrVec_Add(v, Str_ToHex(m, rand));

    ctx->metrics.count++;
    StrVec_Add(v, S(m, "-"));
    StrVec_Add(v, Str_FromI64Pad(m, ctx->metrics.count, 10));

    return v;
}

status Ssid_Open(SsidCtx *ctx, StrVec *ssid, StrVec *ua){
    StrVec *pathV = StrVec_From(ctx->m, ctx->path);
    MemCh *m = ctx->m;

    Str *parityHex = Span_Get(ssid->p, 0);
    quad parity = 0;
    Raw_FromHex(m, parityHex, &parity, sizeof(parity));
    if(!Parity_Compare(parity, ua)){
        ssid->type.state |= (NOOP|LAST);
        return ssid->type.state;
    }

    StrVec_AddVec(pathV, ssid);
    Str *path = StrVec_Str(m, pathV);
    if(Dir_Exists(ctx->m, path) & SUCCESS){
        ssid->type.state |= SUCCESS;
    }else{
        ssid->type.state |= Dir_Mk(ctx->m, path);
    }

    return ssid->type.state;
}

StrVec *Ssid_Update(SsidCtx *ctx, StrVec *ssid, StrVec *ua, microTime time){
    MemCh *m = ctx->m;
    if(Ssid_Open(ctx, ssid, ua) & SUCCESS){
        StrVec *pathV = StrVec_From(ctx->m, ctx->path);
        StrVec_AddVec(pathV, ssid);
        Str *path = StrVec_Str(m, pathV);

        StrVec *new = Ssid_From(ctx, ua, time); 
        pathV = StrVec_From(ctx->m, ctx->path);
        StrVec_AddVec(pathV, new);
        Str *newPath = StrVec_Str(m, pathV);
        File_Rename(ctx->m, newPath, path);
        return new;
    }
    return NULL;
}

StrVec *Ssid_Start(SsidCtx *ctx, StrVec *ua, microTime time){
    MemCh *m = ctx->m;
    StrVec *pathV = StrVec_From(ctx->m, ctx->path);

    StrVec *ssid = Ssid_From(ctx, ua, time); 
    StrVec_AddVec(pathV, ssid);

    Str *path = StrVec_Str(m, pathV);
    if(Dir_Exists(ctx->m, path)){
        return NULL;
    }

    ssid->type.state |= Dir_Mk(ctx->m, path);
    if(ssid->type.state & SUCCESS){
        return ssid;
    }
    return NULL;
}

void *Ssid_Get(SsidCtx *ctx, StrVec *ssid, StrVec *key){
    MemCh *m = ctx->m;

    StrVec *pathV = StrVec_From(ctx->m, ctx->path);
    StrVec_AddVec(pathV, ssid);
    Str *path = StrVec_Str(m, pathV);
    if(Dir_Exists(ctx->m, path)){
        StrVec_Add(pathV, S(m, "/"));
        StrVec_AddVec(pathV, key);
        path = StrVec_Str(m, pathV);
        Buff *bf = Buff_Make(m, ZERO);
        if(File_Exists(bf, path)){
            File_Open(bf, path, O_RDONLY);
            Buff_Read(bf);
            File_Close(bf);
            return bf->v;
        }
    }
    return NULL;
}

status Ssid_UnSet(SsidCtx *ctx, StrVec *ssid, StrVec *key){
    MemCh *m = ctx->m;

    StrVec *pathV = StrVec_From(ctx->m, ctx->path);
    StrVec_AddVec(pathV, ssid);
    Str *path = StrVec_Str(m, pathV);
    if(Dir_Exists(ctx->m, path)){
        StrVec_Add(pathV, S(m, "/"));
        StrVec_AddVec(pathV, key);
        return File_Unlink(m, StrVec_Str(m, pathV));
    }
    return NOOP;
}

status Ssid_Set(SsidCtx *ctx, StrVec *ssid, StrVec *key, void *value){
    MemCh *m = ctx->m;

    StrVec *pathV = StrVec_From(ctx->m, ctx->path);
    StrVec_AddVec(pathV, ssid);
    Str *path = StrVec_Str(m, pathV);
    if(Dir_Exists(ctx->m, path)){
        StrVec_Add(pathV, S(m, "/"));
        StrVec_AddVec(pathV, key);
        Buff *bf = Buff_Make(m, BUFF_UNBUFFERED);
        File_Open(bf, StrVec_Str(m, pathV), O_CREAT|O_TRUNC|O_WRONLY);
        Abstract *a = value;
        ToS(bf, a, a->type.of, ZERO);
        File_Close(bf);
        return SUCCESS;
    }
    return NOOP;
}

status Ssid_Close(SsidCtx *ctx, StrVec *ssid, StrVec *ua){
    StrVec *pathV = StrVec_From(ctx->m, ctx->path);
    MemCh *m = ctx->m;

    Str *parityHex = Span_Get(ssid->p, 0);
    quad parity = 0;
    Raw_FromHex(m, parityHex, &parity, sizeof(parity));
    if(!Parity_Compare(parity, ua)){
        ssid->type.state |= (NOOP|LAST);
        return ssid->type.state;
    }

    StrVec_AddVec(pathV, ssid);
    Str *path = StrVec_Str(m, pathV);
    if(Dir_Exists(ctx->m, path) & SUCCESS){
        ssid->type.state |= SUCCESS;
        return Dir_Rm(ctx->m, path);
    }
    return NOOP;
}

SsidCtx *SsidCtx_Make(MemCh *m, Str *path){
    SsidCtx *ctx = (SsidCtx *)MemCh_AllocOf(m, sizeof(SsidCtx), TYPE_SSID_CTX);
    ctx->type.state = TYPE_SSID_CTX;
    ctx->m = m;
    ctx->path = path;
    return ctx;
}
