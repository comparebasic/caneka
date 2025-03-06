#include <external.h>
#include <caneka.h>

Span *FmtItem_GetChildren(MemCtx *m, FmtItem *item){
    if(item->children == NULL){
        item->children = Span_Make(m);
    }

    return item->children;
}

FmtItem *FmtItem_Make(MemCtx *m, FmtCtx *o){
    FmtItem *fi =  MemCtx_Alloc(m, sizeof(FmtItem));
    fi->type.of = TYPE_FMT_ITEM;
    fi->content = String_Init(m, STRING_EXTEND);
    return fi;
}
