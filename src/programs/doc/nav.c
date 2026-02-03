#include <external.h>
#include <caneka.h>
#include <doc_module.h>

void Doc_GenNav(NodeObj *config, Span *files, WwwNav *nav){
    MemCh *m = config->m;

    Iter it;
    Iter_Init(&it, files);
    while((Iter_Next(&it) & END) == 0){
        StrVec *file = Iter_Get(&it);
        StrVec *route = StrVec_SubVec(m,
            file,
            Path_FlagIdx(file, (MORE|NOOP)),
            Path_FlagIdx(file, (LAST))-1);

        Str *div = Span_Get(route->p, 0);
        div->type.state &= ~NOOP;

        Str *name = Str_Clone(m, Span_Get(route->p, route->p->max_idx)); 
        Str_ToTitle(m, name);

        Table *coords = Inst_Att(nav, K(m, "coords"));

        WwwNav *item = WwwNav_Make(m, route, StrVec_From(m, name));
        Inst_SetAtt(item, K(m, "fpath"), file);

        void *ar[] = {item, NULL};
        Out("^p.Item @^0\n", ar);

        WwwNav_Add(nav, item, coords);
    }

    return;
}
