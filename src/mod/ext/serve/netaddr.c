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

void NetAddr_SetFromStr6(MemCh *m, NetAddr *addr, Str *s){
    if(addr->type.of != TYPE_NET_ADDR6){
        void *ar[] = {addr, NULL};
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "Trying to set ip6 of another address type", ar);
        return;
    }

    status r = READY;

    Str *s6 = Str_Make(m, sizeof(util)*2);
    s6->type.state |= STRING_BINARY;
    
    memset(&addr->net.ip6addr.sin6_addr, 0, IP6_ALLOC);
    byte *ptr = (byte *)&addr->net.ip6addr.sin6_addr;
    byte *sec = ptr;
    byte *b = s->bytes;
    byte *end = s->bytes+s->length-1;
    Str *ref = Str_Ref(m, s->bytes, 0, s->length, STRING_CONST);

    i32 zstart = 0;
    i32 zend = 0;

    while(b <= end){
        if(*b != ':'){
            ref->length++;
        }

        if(*b == ':' || b == end){
            if(ref->length == 0){
                zstart = zend;
            }else{
                zend++;
                ptr = sec+1;
                while(ref->length){
                    byte c = ref->bytes[ref->length-1];
                    *ptr = c >= 'a' ?  
                            c - 'a' + 10:
                            c - '0';
                    Str_Decr(ref, 1);
                    if(ref->length){
                        c = ref->bytes[ref->length-1];
                        *ptr |= c >= 'a' ?  
                            ((c - 'a') + 10) << 4:
                            (c - '0') << 4;
                        Str_Decr(ref, 1);
                    }
                    ptr--;
                }
                sec += 2;
            }
            ref->bytes = b+1;
            ref->alloc -= (ref->length+1);
            ref->length = 0;
        }

        b++;
    }

    if(zstart != 0 || zend != 8){
        ptr = (byte *)&addr->net.ip6addr.sin6_addr;
        i32 length = (zend-zstart)*2;
        memmove(ptr+(IP6_ALLOC-length), ptr+(zstart*2), length);
        memset(ptr+(zstart*2), 0, length);
    }
}
