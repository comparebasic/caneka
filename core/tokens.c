#include <external.h>
#include <filestore.h>

String *space_tk;
int *methods;

status Tokens_Init(MemCtx *m){
    space_tk = String_From(m, bytes(" "));
    return SUCCESS;
}
