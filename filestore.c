#include "external.h"
#include "filestore.h"

char *State_ToString(status state){
    if(state == READY){
        return "READY";
    }else if(state == SUCCESS){
        return "SUCCESS";
    }else if(state == ERROR){
        return "ERROR";
    }else if(state == NOOP){
        return "NOOP";
    }else if(state == INCOMING){
        return "INCOMING";
    }else if(state == PROCESSING){
        return "PROCESSING";
    }else if(state == RESPONDING){
        return "RESPONDING";
    }else{
        return "UNKNOWN_state";
    }
}
