#include <external.h>
#include <caneka.h>

HostEnt *HostEnt_Make(MemCh *m){
    HostEnt *h = MemCh_AllocOf(m, sizeof(HostEnt), TYPE_HOST_ENT);
    h->type.of = TYPE_HOST_ENT;
    return h;
}

HostEnt *HostEnt_FromName(MemCh *m, Str *s){
    HostEnt *h = HostEnt_Make(m);
    h->name = s;
    h->ent = gethostbyname(Str_Cstr(m, s));
    h->addrs = Span_Make(m);
    if(h->ent == NULL){
        h->type.state |= ERROR;
    }else if(h->ent->h_length > 0){
        i32 **list = (i32 **)h->ent->h_addr_list;
        while(*list != NULL){
            i32 *ptr = *list;
            Span_Add(h->addrs, I32_Wrapped(m, *ptr));
            list++;
        }
    }
    return h;
}

quad *HostEnt_AddrIp4(HostEnt *h){
    if(h->type.state & ERROR){
        return 0;
    }
    Single *sg = Span_Get(h->addrs, 0);
    return &sg->val.i;
}

util *HostEnt_AddrIp6(HostEnt *h){
    return NULL;
}
