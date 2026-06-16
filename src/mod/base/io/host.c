#include <external.h>
#include "base_module.h"

HostEnt *HostEnt_Make(MemCh *m){
    HostEnt *h = MemCh_AllocOf(m, sizeof(HostEnt), TYPE_HOST_ENT);
    h->type.of = TYPE_HOST_ENT;
    return h;
}

HostEnt *HostEnt_FromName(MemCh *m, Str *s){
    HostEnt *h = HostEnt_Make(m);
    h->ent = gethostbyname(Str_Cstr(m, s));
    if(h->ent->h_length > 0){
        h->addrs = Span_Make(m);
        i32 **list = (i32 **)h->ent->h_addr_list;
        while(*list != NULL){
            i32 *ptr = *list;
            Span_Add(h->addrs, I32_Wrapped(m, *ptr));
            list++;
        }
    }
    return h;
}
