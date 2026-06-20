#include <external.h>
#include <caneka.h>

TcpSource *TcpSource_Make(MemCh *m){
    TcpSource *ts = MemCh_AllocOf(m, sizeof(TcpSource), TYPE_TCP_SOURCE);
    ts->type.of = TYPE_TCP_SOURCE;
    return ts;
}
