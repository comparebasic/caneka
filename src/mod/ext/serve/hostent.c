#include <external.h>
#include <caneka.h>

HostEnt *HostEnt_Make(MemCh *m){
    HostEnt *h = MemCh_AllocOf(m, sizeof(HostEnt), TYPE_HOST_ENT);
    h->type.of = TYPE_HOST_ENT;
    return h;
}

HostEnt *HostEnt_FromName(MemCh *m, Str *name, Str *service){
    HostEnt *h = HostEnt_Make(m);
    h->name = name;
    h->addrs = Span_Make(m);
    if(getaddrinfo(Str_Cstr(m, name), Str_Cstr(m, service), NULL, &h->info) != 0){
        void *ar[] = {
            name, service, NULL
        };
        Error(m, FUNCNAME, FILENAME, LINENUMBER, 
            "Error getting network address info for $/$", ar);
        h->type.state |= ERROR;
    }else{
        struct addrinfo *info = h->info;
        while(info != NULL){
            if(info->ai_family == AF_INET){
                struct sockaddr_in *addr = (struct sockaddr_in *)info->ai_addr;
                quad ip4 = addr->sin_addr.s_addr;
                Span_Add(h->addrs, U32_Wrapped(m, ip4));
            }
            info = info->ai_next;
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

util HostEnt_Hash(void *a){
    HostEnt *ent = (HostEnt *)a;
    Single *sg = Span_Get(h->addrs, 0);
    return (util)sg->val.i;
}

util *HostEnt_AddrIp6(HostEnt *h){
    return NULL;
}
