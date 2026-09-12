#include <external.h>
#include "buildeka_module.h"

void BuildCtx_Config(BuildCtx *ctx){
    MemCh *m = ctx->m;
    Debug_Push(m, ctx);

    BuildModule *md = NULL;

    Iter it;
    Iter_Init(&it, ctx->options);
    while((Iter_Next(&it) & END) == 0){
        Abstract *a = Iter_Get(&it);
        if(Ident_Check(a) & SUCCESS){
            Iter_Set(&it, Ident_FromVec(m, Ifc(m, a, TYPE_STRVEC)));
        }
    }

    StrVec *configPath = StrVec_Make(m);
    if(ctx->ident != NULL){
        md = BuildModule_FromIdent(MemCh_Make(), ctx, ctx->ident);
        BuildModule_Load(ctx, md);
    }
    
    Table_Set(ctx->deps, Ident_NameStr(m, ctx->ident), md);
    ctx->depsOrdered = Table_Ordered(m, ctx->deps);

    ReturnVoid(m);
}
