#include <external.h>
#include <caneka.h>

status Mess_Append(Mess *set, Node *node, Abstract *a){
    if(a->type.of == TYPE_MESS){
        Node *child = (Node *)a;
        child->parent = node;
    }
    if((node->type.state & (FLAG_CHILD|FLAG_CHILDREN)) == 0){
        node->child.value = a;
        node->type.state |= FLAG_CHILD;
        return node->type.state;
    }else if((node->type.state & FLAG_CHILD) == 0){
        Abstract *value = node->child.value;
        node->child.items = Span_Make(set->m);
        Span_Add(node->child.items, value);
    }

    Span_Add(node->child.items, a);
    return node->type.state;
}

status Mess_AddAtt(Mess *mess, Node *node, Abstract *key, Abstract *value){
    if(node->atts == NULL){
        node->atts = Table_Make(mess->m); 
    } 
    return Table_Set(node->atts, key, value);
}

Mess *Mess_Make(MemCh *m){
    Mess *mess = (Mess *)MemCh_Alloc(m, sizeof(Mess));
    mess->type.of = TYPE_MESS;
    mess->m = m;
    mess->root = Node_Make(m, 0);
    return mess;
}
