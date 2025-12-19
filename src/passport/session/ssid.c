#include <external.h>
#include <caneka.h>

StrVec *Ssid_From(SsidCtx *ctx, StrVec *ua, struct timespec *ts){
    MemCh *m = ctx->m;
    quad parity = HalfParity_FromVec(ua);
    StrVec *v = StrVec_Make(m);

    ctx->metrics.count++;
    StrVec_Add(v, Str_UniRandom(m, ctx->metrics.count, 8));

    StrVec_Add(v, Str_FromCstr(m, "-", STRING_COPY|MORE));
    StrVec_Add(v, Str_FromI64(m,ts->tv_sec));
    StrVec_Add(v, Str_FromCstr(m, ".", STRING_COPY|MORE));
    StrVec_Add(v, Str_FromI64(m,ts->tv_nsec));

    StrVec_Add(v, Str_FromCstr(m, "-", STRING_COPY|MORE));
    StrVec_Add(v,
        Str_ToHex(m,
            Str_Ref(m, (byte *)&parity, sizeof(parity), sizeof(parity), STRING_BINARY)));

    return v;
}

Table *Ssid_Open(SsidCtx *ctx, StrVec *ssid, StrVec *ua){
    StrVec *pathV = StrVec_From(ctx->m, ctx->path);
    MemCh *m = ctx->m;

    Str *parityHex = Span_Get(ssid->p, 6);
    quad parity = 0;
    Raw_FromHex(m, parityHex, &parity, sizeof(parity));
    if(!HalfParity_Compare(parity, ua)){
        ssid->type.state |= (NOOP|LAST);
        return NULL;
    }

    StrVec_AddVec(pathV, ssid);
    Str *path = StrVec_Str(m, pathV);
    if(Dir_Exists(ctx->m, path) & SUCCESS){
        StrVec_Add(pathV, S(m, "/session.stash"));
        Buff *bf = Buff_Make(m, ZERO);
        File_Open(bf, StrVec_Str(m, pathV), O_RDONLY);
        MemCh *stash = Stash_FromStream(bf);
        File_Close(bf);
        if(stash != NULL && stash->owner != NULL &&
                ((Abstract *)stash->owner)->type.of == TYPE_TABLE){
            ssid->type.state |= SUCCESS;
            return stash->owner;
        }else{
            void *args[] = {ssid, NULL};
            Error(m, FUNCNAME, FILENAME, LINENUMBER,
                "Stash data for session $ is NULL", args);
        }
    }else{
        ssid->type.state |= NOOP;
    }

    return NULL;
}

StrVec *Ssid_Update(SsidCtx *ctx, StrVec *ssid, StrVec *ua, struct timespec *ts){
    MemCh *m = ctx->m;
    ssid->type.state &= ~(SUCCESS|NOOP);
    Ssid_Open(ctx, ssid, ua);
    if(ssid->type.state & SUCCESS){
        StrVec *pathV = StrVec_From(ctx->m, ctx->path);
        StrVec_AddVec(pathV, ssid);
        Str *path = StrVec_Str(m, pathV);

        StrVec *new = Ssid_From(ctx, ua, ts); 
        pathV = StrVec_From(ctx->m, ctx->path);
        StrVec_AddVec(pathV, new);
        Str *newPath = StrVec_Str(m, pathV);
        File_Rename(ctx->m, newPath, path);
        return new;
    }
    return NULL;
}

StrVec *Ssid_Start(SsidCtx *ctx, StrVec *ua, struct timespec *ts){
    MemCh *m = ctx->m;
    StrVec *pathV = StrVec_From(ctx->m, ctx->path);

    StrVec *ssid = Ssid_From(ctx, ua, ts); 
    StrVec_AddVec(pathV, ssid);

    Str *path = StrVec_Str(m, pathV);
    if(Dir_Exists(ctx->m, path) & SUCCESS){
        return NULL;
    }

    ssid->type.state |= Dir_Mk(ctx->m, path);
    if(ssid->type.state & SUCCESS){
        MemCh *pst = MemCh_Make();
        Table *dict = Table_Make(pst);
        Table_Set(dict, S(pst, "user-agent"), StrVec_Str(pst, ua));
        Table_Set(dict, S(pst, "orig-ssid"), StrVec_Str(pst, ssid));
        pst->owner = dict;

        StrVec_Add(pathV, S(m, "/session.stash"));
        Buff *bf = Buff_Make(m, ZERO);
        File_Open(bf, StrVec_Str(m, pathV), O_WRONLY|O_CREAT|O_TRUNC);
        if(Stash_FlushFree(bf, pst) & SUCCESS){
            File_Close(bf);
            return ssid;
        }
        File_Close(bf);
    }
    return NULL;
}

status Ssid_Close(SsidCtx *ctx, StrVec *ssid, StrVec *ua, Table *stashTbl){
    ssid->type.state &= ~(SUCCESS|NOOP);

    StrVec *pathV = StrVec_From(ctx->m, ctx->path);
    MemCh *m = ctx->m;
    Str *parityHex = Span_Get(ssid->p, 6);
    quad parity = 0;
    Raw_FromHex(m, parityHex, &parity, sizeof(parity));
    if(!HalfParity_Compare(parity, ua)){
        ssid->type.state |= (NOOP|ERROR);
        return ssid->type.state;
    }

    StrVec_AddVec(pathV, ssid);
    Str *path = StrVec_Str(m, pathV);
    if(Dir_Exists(ctx->m, path) & SUCCESS){
        StrVec_Add(pathV, S(m, "/session.stash"));
        Buff *bf = Buff_Make(m, BUFF_CLOBBER);
        File_Open(bf, StrVec_Str(m, pathV), O_WRONLY|O_CREAT|O_TRUNC);
        if(Stash_FlushFree(bf, stashTbl->m) & SUCCESS){
            File_Close(bf);
            ssid->type.state |= SUCCESS;
            return ssid->type.state;
        }
        File_Close(bf);
    }

    ssid->type.state |= NOOP;
    return ssid->type.state;
}

status Ssid_Destroy(SsidCtx *ctx, StrVec *ssid, StrVec *ua){
    StrVec *pathV = StrVec_From(ctx->m, ctx->path);
    MemCh *m = ctx->m;
    Str *parityHex = Span_Get(ssid->p, 6);
    quad parity = 0;
    Raw_FromHex(m, parityHex, &parity, sizeof(parity));
    if(!HalfParity_Compare(parity, ua)){
        ssid->type.state |= (NOOP|ERROR);
        return ssid->type.state;
    }

    StrVec_AddVec(pathV, ssid);
    StrVec_Add(pathV, S(m, "/session.stash"));
    File_Unlink(m, StrVec_Str(m, pathV));
    StrVec_Pop(pathV);

    Str *path = StrVec_Str(m, pathV);
    if(Dir_Exists(ctx->m, path) & SUCCESS){
        ssid->type.state |= Dir_Rm(ctx->m, path);
        return ssid->type.state;
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
