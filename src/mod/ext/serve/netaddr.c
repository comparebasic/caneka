#include <external.h>
#include <caneka.h>

NetAddr *NetAddr_Make4(MemCh *m){
    NetAddr *addr = MemCh_AllocOf(m, sizeof(NetAddr), TYPE_NET_ADDR4);
    addr->type.of = TYPE_NET_ADDR4;
    return addr;
}

NetAddr *NetAddr_Make6(MemCh *m){
    NetAddr *addr = MemCh_AllocOf(m, sizeof(NetAddr), TYPE_NET_ADDR6);
    addr->type.of = TYPE_NET_ADDR6;
    return addr;

}
