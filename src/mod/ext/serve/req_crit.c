#include <external.h>
#include <caneka.h>


boolean ReqCrit_Func(Queue *q, Req *req, ReqCrit *crit){
    if(crit->until.tv_sec > 0 && Time_IsPast(&q->time.present, &crit->until)){
        return TRUE; 
    }else{
        return crit->pfd.fd != -1 && 
            ((crit->pfd.events & (POLLIN|POLLOUT)) == 0 || poll(&crit->pfd, 1, 0) == 1);
    }
}

ReqCrit *ReqCrit_Make(MemCh *m){
    ReqCrit *crit = MemCh_AllocOf(m, sizeof(ReqCrit), TYPE_REQ_CRIT);
    crit->type.of = TYPE_REQ_CRIT;
    crit->pfd.fd = -1;
    return crit;
}
