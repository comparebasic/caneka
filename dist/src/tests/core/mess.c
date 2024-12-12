#include <external.h>
#include <caneka.h>

static status mess_Extend(Mess *existing, Mess *new){
    if(existing != NULL){
        while(existing->next != NULL){
            existing = existing->next;
        }
        existing->next = new;
        return SUCCESS;
    }

    return ERROR;
}


Mess *Mess_Make(MemCtx *m){
    Mess *ms = (Mess *)MemCtx_Alloc(m, sizeof(Mess));
    ms->type.of = TYPE_MESS;
    return ms;
}

status Mess_Append(Mess *existing, Mess *ms){
    if(existing != NULL && ms != existing){
        if(existing->firstChild == NULL){
            existing->firstChild = ms;
        }else{
            mess_Extend(existing->firstChild, ms);
        }
        ms->parent = existing;
        return SUCCESS;
    }

    printf("OOps they are identical\n");

    return ERROR;
}
