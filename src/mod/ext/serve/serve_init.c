#include <external.h>
#include <caneka.h> 

static Table *services = NULL;

void Serve_Init(MemCh *m){
    if(services == NULL){
        services = Table_Make(m);
        Table_Set(services, S(m, "http"), I32_Wrapped(m, 80));
        Table_Set(services, S(m, "https"), I32_Wrapped(m, 443));
    }
}
