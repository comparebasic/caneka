#include <external.h>
#include <caneka.h>

SendRecv *Recv_Make(m, i32 fd, StrVec *v, word flags){
    SendRecv *sr = (SendRecv *)MemCh_Alloc(m, sizeof(SendRecv), TYPE_RECV);
    sr->type.of = TYPE_RECV;
    sr->type.state = flags;
    sr->fd = fd;
    if(v == NULL){
        v = StrVec_Make(m);
    }
    sr->v = v;
    return sr;
}
