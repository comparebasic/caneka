#include <external.h>
#include <caneka.h>


Uri *Uri_Make(MemCh *m, StrVec *v){
    Uri *uri = MemCh_AllocOf(m, sizeof(Uri), TYPE_URI);
    uri->v = v;
    uri->type.of = TYPE_URI;
    uri->path = StrVec_Make(m);
    uri->host = StrVec_Make(m);
    Iter_Init(&uri->queryIt, Table_Make(m));
    Roebling *rbl = UriRbl_Make(m, Cursor_Make(m, v), uri);
    rbl->type.state |= DEBUG;
    Roebling_Run(rbl);
    Roebling_Finalize(rbl, NULL, 0);
    uri->type.state |= rbl->type.state & (SUCCESS|ERROR);
    return uri;
}
