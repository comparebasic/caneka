#include <external.h>
#include <caneka.h>

Templ *Templ_Make(MemCh *m, Span *content, Span *data){
    Templ templ = (Templ *)MemCh_Alloc(m, sizeof(Templ));
    templ->type.of = TYPE_TEMPL;
    Iter_Init(&templ->content, content);
    Iter_Init(&templ->nested, data);
    return templ;
}

Templ *Templ_ToS(Templ *templ, Stream *sm){
    if(Iter_Next(&templ->content) & END){
        templ->type.state |= SUCCESS;
        return templ->type.state;
    }
    Abstract *item = Iter_Get(&templ->content);
    Abstract *data = Iter_Current(&templ->data);

    if(item->type.of > _FORMAT_CASH_VAR_ENDINGS && 
            frame->type.of < _FORMAT_CASH_VAR_ENDINGS_END){
        /* wind back */;
    }else if(item->type.of == TYPE_ITER){
        /* select next item */
    }
}
