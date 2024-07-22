#include <external.h>
#include <caneka.h>

int Header_GetInt(Span *hdr_tbl, String *key){
    String *hdr = (String *)Table_Get(hdr_tbl, (Abstract *)key);
    if(hdr != NULL){
        return String_ToInt(hdr); 
    }

    return 0;
}
