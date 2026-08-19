#include <external.h>
#include <caneka.h>

void Doc_GenFromFile(Node *ctx, DocInst *d, Buff *bf){
    return;
}

DocInst *DocInst_Make(MemCh *m, Node *ctx, StrVec *src, StrVec *out){
    return NULL;
}

void Doc_GenNav(Node *ctx){
    MemCh *m = ctx->m;
    Table *navCoords = Table_Make(m);
    Node *navNode = Inst_Make(m, TYPE_NODE);

    Str *dir = Ifc(m, Inst_GetChild(ctx, S(m, "dir")), TYPE_STR);

    Span *files = Span_Make(m);
    Span *dest = Span_Make(m);
    DirSel *sel = DirSel_Make(m, NULL, files, ZERO);
    Span *exts = Span_Make(m);
    Span_Add(exts, S(m, "c"));
    Span_Add(exts, S(m, "h"));

    Dir_GatherByExt(m, Ifc(m, dir, TYPE_STR), files, exts);

    Iter it;
    Iter_Init(&it, files);
    while((Iter_Next(&it) & END) == 0){
        StrVec *path = IoPath_FromVec(m, Iter_Get(&it));
        StrVec *local = Clone(m, path);
        StrVec_Incr(local, dir->length+1);
        Span *p = IoPath_ToInstPath(m, local); 

        Span *coords = Span_Make(m);
        Node *page = Inst_Make(m, TYPE_NODE);
        Inst_SetChild(page, S(m, "srcPath"), path);
        Inst_SetChild(page, S(m, "coords"), coords);
        Inst_ByPath(navNode, p, page, SPAN_OP_SET, coords);
        Table_Set(navCoords, local, coords);


        void *ar[] = {
            local,
            coords,
            NULL
        };
        Out("^c.Local @^0 -> ^p.@^0\n", ar);
    }

    Inst_SetChild(ctx, S(m, "navCoords"), navCoords);
    Inst_SetChild(ctx, S(m, "nav"), navNode);
}
