#include <external.h>
#include <caneka.h>
#include <doc_module.h>

void Doc_Gather(Table *tbl, Span *files){
    MemCh *m = tbl->m;
    Iter fileIt;
    Iter_Init(&fileIt, files);

    Iter it;
    Iter_Init(&it, tbl);
    while((Iter_Next(&it) & END) == 0){
        Hashed *h = Iter_Get(&it);
        if(h != NULL){

            DirSel *sel = DirSel_Make(m, NULL, NULL, DIR_SELECTOR_NODIRS);
            sel->source = Span_Make(m);

            NodeObj *node = h->value;
            sel->ext = StrVec_Str(m, Inst_Att(node, K(m, "ext")));
            StrVec *path = IoUtil_AbsVec(m, h->key);
            Str *pathS = StrVec_Str(m, path);
            Span_Set(sel->source, 0, pathS);

            Dir_GatherFilterDir(m, pathS, sel);

            Iter selIt;
            Iter_Init(&selIt, sel->dest);
            while((Iter_Next(&selIt) & END) == 0){
                StrVec *file = Iter_Get(&selIt);
                IoUtil_Annotate(m, file);
                Str *div =  Span_Get(file->p, path->p->max_idx);
                div->type.state |= NOOP;
                Iter_Add(&fileIt, file);
            }
        }
    }
}
