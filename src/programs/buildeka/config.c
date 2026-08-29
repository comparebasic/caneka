#include <external.h>
#include "buildeka_module.h"

void BuildCtx_Config(BuildCtx *ctx){
    MemCh *m = ctx->m;

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
        if(Ident_Check(m, a) & SUCCESS){
            Iter_Set(&it, Ident_FromVec(m, Ifc(m, a, TYPE_STRVEC)));
        }
    }

    ctx->deps = Table_Make(m);
    Table *deps = Node_KvFromChild(config, K(m, "dependency")); 

    Iter_Init(&it, deps);
    while((Iter_Next(&it) & END) == 0){
        Hashed *h = Iter_Get(&it);
        if(h != NULL){
            if(Equals(h->value, K(m, "option")) || Equals(h->value, K(m, "implied option"))){
                if(Span_Has(ctx->options, h->key) != -1){
                    Table_Set(ctx->deps, h->key, BuildModule_Make(m, h->key));
                }
            }else{
                Table_Set(ctx->deps, h->key, BuildModule_Make(m, h->key));
            }
        }
    }

    void *ar[] = {
        config,
        ctx,
        NULL
    };
    Out("^y.Config @\n^c.@^0\n", ar);
}
