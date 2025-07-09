#include <external.h>
#include <caneka.h>

Templ *Templ_Make(MemCh *m, Span *content, OrdTable *data){
    Templ *templ = (Templ *)MemCh_Alloc(m, sizeof(Templ));
    templ->type.of = TYPE_TEMPL;
    Iter_Init(&templ->content, content);
    Span *p = Span_Make(m);
    Span_Add(p, (Abstract *)data);
    Iter_Init(&templ->data, p);
    Iter_Next(&templ->data);
    return templ;
}

i64 Templ_ToSCycle(Templ *templ, Stream *sm, i64 total){
    if(Iter_Next(&templ->content) & END){
        templ->type.state |= SUCCESS;
        return total;
    }
    Abstract *item = Iter_Get(&templ->content);
    Abstract *data = Iter_Current(&templ->data);

    if(item->type.of > _FORMAT_CASH_VAR_ENDINGS && 
            item->type.of < _FORMAT_CASH_VAR_ENDINGS_END){
        /* wind back */;
    }else if(item->type.of == TYPE_ITER){
        /* select next item */
    }else if(item->type.of == TYPE_STRVEC){
        total += ToS(sm, (Abstract *)item, 0, ZERO); 
    }else if(item->type.of == TYPE_WRAPPED_PTR){
        Single *sg = (Single *)item;
        if(sg->objType.of == FORMAT_CASH_VAR_FOR){
            printf("FOR\n");
            fflush(stdout);
            Iter_Add(&templ->data, (Abstract *)Iter_Make(sm->m, ));
        }else if(sg->objType.of == FORMAT_CASH_VAR_KEYVALUE){
            Hashed *h = FromKeyHashed(data, sg->val.ptr);
            Abstract *value = Name((Abstract *)h);
            total += ToS(sm, (Abstract *)value, 0, ZERO); 
        }else if(sg->objType.of == FORMAT_CASH_VAR_NAMEVALUE){
            Hashed *h = FromKeyHashed(data, sg->val.ptr);
            Abstract *value = Value((Abstract *)h);
            total += ToS(sm, (Abstract *)value, 0, ZERO); 
        }else if(sg->objType.of == FORMAT_CASH_VAR_KEY){
            Abstract *value = FromKey(data, sg->val.ptr);
            total += ToS(sm, (Abstract *)value, 0, ZERO); 
        }
    }

    if(templ->content.type.state & END){
        templ->type.state |= SUCCESS;
    }

    if(templ->type.state & DEBUG){
        Abstract *args[] = {
            (Abstract *)templ,
            (Abstract *)item,
            (Abstract *)data,
            NULL
        };
        Out("^p.Templ: &\n^0.^y&\n^b.@^0\n", args);
    }

    return total;
}

i64 Templ_ToS(Templ *templ, Stream *sm){
    i64 total = 0;
    i16 g = 0;
    while((total = Templ_ToSCycle(templ, sm, total)) && 
        (templ->type.state & OUTCOME_FLAGS) == 0){
        Guard_Incr(&g, 64, FUNCNAME, FILENAME, LINENUMBER);
    }
    return total;
}
