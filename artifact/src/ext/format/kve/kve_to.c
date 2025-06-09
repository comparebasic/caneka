#include <external.h>
#include <caneka.h>

status Kve_ToTable(MemCtx *m, String *kve, Span *tbl){
   Roebling *rbl = Kve_RblMake(m, kve, (Abstract *)tbl, Kve_Capture); 
   return Roebling_Run(rbl);
}
