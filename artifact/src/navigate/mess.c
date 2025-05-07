#include <external.h>
#include <caneka.h>

status Mess_Tokenize(Mess *mess, Tokenize *tk, StrVec *v){
    DebugStack_Push(NULL, 0);
    Abstract *a = NULL;
    Node *nd = NULL;
    cls typeOf = 0;
    if(tk->type.state & TOKEN_CLOSE_OUTDENT){
        mess->current = mess->current->parent; 
        if(tk->typeOf == 0){
            return mess->type.state;
        }
    }
    if(tk->typeOf == TYPE_NODE){
        nd = Node_Make(mess->m, 0);
        nd->captureKey = tk->captureKey;
        a = (Abstract *)nd;
    }else if(tk->typeOf == TYPE_STRVEC){
        a = (Abstract *)v;
        typeOf = TYPE_STRVEC;
    }
    Mess_GetOrSet(mess, mess->current, a, typeOf);
    DebugStack_Pop();
    return mess->type.state;
}

status Mess_GetOrSet(Mess *mess, Node *node, Abstract *a, cls typeOf){
    if(node == NULL){
        Abstract *args[] = {
            (Abstract *)mess,
            NULL
        };
        Error(mess->m, (Abstract *)mess, FUNCNAME, FILENAME, LINENUMBER,
            "Node is NULL for mess @", args);
        return ERROR;
    }
    if((node->type.state & (FLAG_CHILD|FLAG_CHILDREN)) == 0){
        node->child.value = a;
        node->type.state |= FLAG_CHILD;
        mess->currentValue = a;
        return node->type.state;
    }else if((node->type.state & FLAG_CHILD) != 0){
        if(typeOf != 0 && mess->currentValue != NULL && mess->current->type.of == typeOf){
            if(Combine((Abstract *)mess->current, a)){
                mess->currentValue = a;
                return SUCCESS;
            }
        }
        Abstract *value = node->child.value;
        node->child.items = Span_Make(mess->m);
        Span_Add(node->child.items, value);
        Span_Add(node->child.items, a);
        mess->currentValue = a;
    }else{ /* FLAG_CHILDREN */
        Span_Add(node->child.items, a);
        mess->currentValue = a;
    }
    return node->type.state;
}

status Mess_Append(Mess *mess, Node *node, Abstract *a){
    if(a->type.of == TYPE_MESS){
        Node *child = (Node *)a;
        child->parent = node;
    }
    return Mess_GetOrSet(mess, node, a, 0);
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
    mess->root = mess->current = Node_Make(m, 0);
    return mess;
}
