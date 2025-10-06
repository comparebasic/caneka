#include <external.h>
#include <caneka.h>

status HttpE_Make(MemCh *m, Handler *chain, Req *req){
    Handler *h = chain;
    h = Handler_Add(m, h, Http_RespondMake, NULL, (Abstract *)req);
    h = Handler_Add(m, h, HttpE_Stats, NULL, (Abstract *)req);
    h = Handler_Add(m, h, Http_RecieveMake, NULL, (Abstract *)req);
    return SUCCESS;
}
