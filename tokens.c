#include "external.h"
#include "filestore.h"

String *space_tk;
String *lineBreak_tk;
String *HTTPV_tk;
String **methodTk;
int *methods;

status Tokens_Init(MemCtx *m){
    space_tk = String_From(m, " ");
    lineBreak_tk = String_From(m, "\r\n");
    HTTPV_tk = String_From(m, "HTTP/1.");

    
    methodTk = MemCtx_Alloc(m, sizeof(String *)*4);
    methods = MemCtx_Alloc(m, sizeof(int)*4);

    methodTk[0] = String_From(m, "GET");
    methods[0] = METHOD_GET;
    methodTk[1] = String_From(m, "SET");
    methods[1] = METHOD_SET;
    methodTk[2] = String_From(m, "UPDATE");
    methods[2] = METHOD_UPDATE;
    methodTk[3] = NULL;
    methods[3] = 0;

    return SUCCESS;
}
