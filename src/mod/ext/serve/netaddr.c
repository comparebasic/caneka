#include <external.h>
#include <caneka.h>

NetAddr *NetAddr_Make4(MemCh *m){
    NetAddr *addr = MemCh_AllocOf(m, sizeof(NetAddr), TYPE_NET_ADDR4);
    addr->type.of = TYPE_NET_ADDR4;
    return addr;
}

void NetAddr_SetFromStr4(NetAddr *net, Str *s){
    if(net->type.of != TYPE_NET_ADDR4){
        void *ar[] = {net, NULL};
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "Trying to set ip4 of another address type", ar);
        return;
    }

    net->ip4addr.sin_addr.s_addr = Str_ToIp4(s);
}

void NetAddr_SetFromStrPort(NetAddr *net, Str *s){
    net->port = ;
}

NetAddr *NetAddr_Make6(MemCh *m){
    NetAddr *addr = MemCh_AllocOf(m, sizeof(NetAddr), TYPE_NET_ADDR6);
    addr->type.of = TYPE_NET_ADDR6;
    return addr;

}
