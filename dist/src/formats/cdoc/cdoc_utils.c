#include <external.h>
#include <caneka.h>

char *CdocRange_ToChars(word range){
    if(range == _CDOC_START){
        return "_CDOC_START";
    }else if(range == CDOC_START){
        return "CDOC_START";
    }else if(range == CDOC_COMMENT){
        return "CDOC_COMMENT";
    }else if(range == CDOC_WS){
        return "CDOC_WS";
    }else if(range == CDOC_TYPE){
        return "CDOC_TYPE";
    }else if(range == CDOC_THING){
        return "CDOC_THING";
    }else if(range == CDOC_END){
        return "CDOC_END";
    }else if(range == _CDOC_END){
        return "_CDOC_END";
    }else{
        return "UNKNOWN";
    }
}
