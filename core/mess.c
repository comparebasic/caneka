#include <external.h>
#include <caneka.h>

Mess *Mess_Make(MemCtx *m){
    Mess *ms = (Mess *)MemCtx_Alloc(m, sizeof(Mess));
    ms->type.of = TYPE_MESS;
    return ms;
}

status Mess_Extend(Mess *new, Mess *existing){
    if(existing != NULL){
        while(existing->next != NULL){
            existing = existing->next;
        }
        existing->next = existing;
        return SUCCESS;
    }

    return ERROR;
}

status Mess_Append(Mess *new, Mess *existing){
    if(existing != NULL){
        new->parent = existing;
        if(existing->firstChild == NULL){
            existing->firstChild = new;
        }else{
            Mess_Extend(new, existing->firstChild);
        }
        return SUCCESS;
    }

    return ERROR;
}
