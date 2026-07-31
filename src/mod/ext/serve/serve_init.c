#include <external.h>
#include <caneka.h> 

static Table *services = NULL;
Table *ServeProtoTable = NULL;

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
        Lookup_Add(m, HashLookup, TYPE_HOST_ENT, (void *)HostEnt_Hash);
    }
    if(ServeProtoTable == NULL){
        ServeProtoTable = Table_Make(m);
        Table_Set(ServeProtoTable, S(m, "https"), HttpTlsReq_DefMake(m));
        Table_Set(ServeProtoTable, S(m, "http"), HttpReq_DefMake(m));
    }
}
