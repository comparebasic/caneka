#include <external.h>
#include "buildeka_module.h"

void BuildCtx_Config(BuildCtx *ctx){
    MemCh *m = ctx->m;
    Debug_Push(m, ctx);

    StrVec *configPath = StrVec_From(m, Span_Get(ctx->input.sources, 0));
    StrVec_Add(configPath, S(m, "/build.json"));


    Node *config = Json_FromPath(m, configPath); 
    if(config == NULL){
        void *ar[] = {
            configPath,
            NULL
        };
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "build.json file not found \\@$", ar);
    }

    Iter it;
    Iter_Init(&it, ctx->options);
    while((Iter_Next(&it) & END) == 0){
        Abstract *a = Iter_Get(&it);
        if(Ident_Check(a) & SUCCESS){
            Iter_Set(&it, Ident_FromVec(m, Ifc(m, a, TYPE_STRVEC)));
        }
    }

    Table *deps = Node_KvFromChild(config, K(m, "dependency")); 

    ctx->deps = Table_Make(m);

    Iter_Init(&it, Table_Ordered(m, deps));
    while((Iter_Next(&it) & END) == 0){
        Hashed *h = Iter_Get(&it);
        if(h != NULL){
            if(Equals(h->value, K(m, "option")) || Equals(h->value, K(m, "implied option"))){
                i32 idx = Span_Has(ctx->options, h->key);
                if(idx != -1){
                    Abstract *opt = Span_Get(ctx->options, idx);
                    BuildModule *md = NULL;
                    if(opt->type.of == TYPE_IDENT){
                        md = BuildModule_FromIdent(m, ctx, (Ident *)opt);
                    }else{
                        md = BuildModule_Make(m, ctx, h->key);
                    }
                    Table_Set(ctx->deps, h->key, md);
                }
            }else{
                Table_Set(ctx->deps, h->key, BuildModule_Make(m, ctx, h->key));
            }
        }
    }
    
    Table_Set(ctx->deps,
        Ident_NameStr(m, ctx->ident),
        BuildModule_FromIdent(m, ctx, ctx->ident));

    void *ar[] = {
        config,
        ctx,
        NULL
    };
    Out("^y.Config @\n^c.@^0\n", ar);

    ReturnVoid(m);
}
