#include <external.h>
#include <caneka.h>

/*
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
*/

status Mess_Append(MessSet *set, Mess *ms, Abstract *key, Abstract *a){
    if(ms->children == NULL){
        ms->children = OrdTable_Make(set->m); 
    } 
    if(a->type.of == TYPE_MESS){
        Mess *child = (Mess *)a;
        child->parent = ms;
    }
    return Table_Set(ms->children->order, key, a);
}

status Mess_AddAtt(Mess *ms, String *key, Abstract *value){
    if(ms->atts == NULL){
        ms->atts = OrdTable_Make(m); 
    } 
    return Table_Set(ms->atts, (Abstract *)key, value);
}

Mess *Mess_Make(MemCtx *m, word tagIdx){
    Mess *ms = (Mess *)MemCtx_Alloc(m, sizeof(Mess));
    ms->type.of = TYPE_MESS;
    ms->tagIdx = tagIdx;
    return ms;
}
