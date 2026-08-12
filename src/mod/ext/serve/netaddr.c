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

void NetAddr_SetFromStr6(MemCh *m, NetAddr *net, Str *s){
    if(net->type.of != TYPE_NET_ADDR6){
        void *ar[] = {net, NULL};
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "Trying to set ip6 of another address type", ar);
        return;
    }

    Str *s6 = Str_Make(m, sizeof(util)*2);
    s6->type.state |= STRING_BINARY;
    
    byte *b = s->bytes;
    byte *end = s->bytes+s->length-1;
    Str *ref = Str_Ref(m, s->bytes, 0, s->length, STRING_CONST);
    while(b <= end){
        if(*b != ':' || b == end){
            ref->length++;
        }

        if(*b == ':'){
            void *ar[] = {
                ref,
                NULL
            };
            Out("^y.Section Found @^0\n", ar);
            ref->bytes = b;
            ref->alloc -= (ref->length+1);
            ref->length = 0;
        }

        b++;
    }

    /*
    net->net.ip4addr.sin_addr.s_addr = Str_ToIp4(m, s);
    */
}
