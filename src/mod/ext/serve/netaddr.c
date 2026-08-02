#include <external.h>
#include <caneka.h>

NetAddr *NetAddr_Make4(MemCh *m){
    NetAddr *addr = MemCh_AllocOf(m, sizeof(NetAddr), TYPE_NET_ADDR4);
    addr->type.of = TYPE_NET_ADDR4;
    addr->net.ip4addr.sin_family = AF_INET;
    return addr;
}

void NetAddr_SetFromStr4(MemCh *m, NetAddr *net, Str *s){
    if(net->type.of != TYPE_NET_ADDR4){
        void *ar[] = {net, NULL};
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "Trying to set ip4 of another address type", ar);
        return;
    }

    net->net.ip4addr.sin_addr.s_addr = Str_ToIp4(m, s);
}

void NetAddr_SetPort(MemCh *m, NetAddr *net, i32 port){
    net->net.ip4addr.sin_port = htons(port);
}

NetAddr *NetAddr_Make6(MemCh *m){
    NetAddr *addr = MemCh_AllocOf(m, sizeof(NetAddr), TYPE_NET_ADDR6);
    addr->type.of = TYPE_NET_ADDR6;
    return addr;

}
