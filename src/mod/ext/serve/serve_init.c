#include <external.h>
#include <caneka.h> 

static Table *services = NULL;

i32 Serve_PortByService(Str *s){
    Single *sg = Table_Get(services, s);
    if(s == NULL){
        return -1;
    }else{
        return sg->val.i;
    }
}

void Serve_Init(MemCh *m){
    if(services == NULL){
        services = Table_Make(m);
        Table_Set(services, S(m, "http"), I32_Wrapped(m, 80));
        Table_Set(services, S(m, "https"), I32_Wrapped(m, 443));
    }
}
