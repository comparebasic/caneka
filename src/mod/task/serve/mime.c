#include <external.h>
#include <caneka.h>

Table *MimeByExt = NULL;

void Mime_Init(MemCh *m){
    if(MimeByExt == NULL){
        MimeByExt = Table_Make(m);
        Table_Set(MimeByExt, S(m, "html"), S(m, "text/html"));
        Table_Set(MimeByExt, S(m, "xml"), S(m, "text/xml"));
        Table_Set(MimeByExt, S(m, "js"), S(m, "text/javascript"));
        Table_Set(MimeByExt, S(m, "png"), S(m, "image/png"));
        Table_Set(MimeByExt, S(m, "jpg"), S(m, "image/jpeg"));
        Table_Set(MimeByExt, S(m, "jpeg"), S(m, "image/jpeg"));
        Table_Set(MimeByExt, S(m, "txt"), S(m, "text/plain"));
        Table_Set(MimeByExt, S(m, "css"), S(m, "text/css"));
    }
}
