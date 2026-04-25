#include <external.h>
#include <caneka.h>

cls Bytes_ToHttpProto(MemCh *m, void *a){
    if(Equals(a, S(m, "HTTP/1.1"))){
        return PROTO_HTTP_1_1;
    }else if(Equals(a, S(m, "HTTP/1.2"))){
        return PROTO_HTTP_1_2;
    }

    return ZERO;
}
