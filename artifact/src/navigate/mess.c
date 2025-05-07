#include <external.h>
#include <caneka.h>

status Mess_Tokenize(Mess *mess, Tokenize *tk, StrVec *v){
    DebugStack_Push(NULL, 0);
    if(tk->type.state & DEBUG){
        Abstract *args[] = {
            (Abstract *)tk,
            NULL
        };
        Out("Mess_Token: @\n", args);
    }
    Abstract *a = NULL;
    Node *nd = NULL;
    cls typeOf = 0;
    if(tk->type.state & TOKEN_CLOSE_OUTDENT){
        if(mess->currentValue != NULL && mess->currentValue != mess->current->child){
            mess->currentValue = mess->current->child;
        }else if(mess->current->parent != NULL){
            mess->current = mess->current->parent; 
            mess->currentValue = mess->current->child;
        }else{
            Abstract *args[] = {
                (Abstract *)mess->current,
                NULL
            };
            Error(mess->m, (Abstract*)mess, FUNCNAME, FILENAME, LINENUMBER,
                "Outdent requested wheere no parent exists, mess->current is $", args);
        }
        return mess->type.state;
    }
    if(tk->typeOf == TYPE_NODE){
        nd = Node_Make(mess->m, 0, mess->current);
        nd->captureKey = tk->captureKey;
        a = (Abstract *)nd;
        typeOf = TYPE_NODE;
        if(tk->type.state & TOKEN_ATTR_VALUE){
            Mess_AddAtt(mess, nd, 
                (Abstract *)I16_Wrapped(mess->m, tk->captureKey), (Abstract *)v);
        }
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
    if(node->typeOfChild == 0){
        node->child = a;
        node->typeOfChild = a->type.of;
        goto end;
    }else if(Combine((Abstract *)mess->currentValue, a)){
        goto end;
    }else if(node->typeOfChild == TYPE_SPAN){
        Span_Add((Span *)node->child, a);
        goto end;
    }else{
        Abstract *value = node->child;
        node->child = (Abstract *)Span_Make(mess->m);
        node->typeOfChild = TYPE_SPAN;
        Span_Add((Span *)node->child, value);
        Span_Add((Span *)node->child, a);
        goto end;
    }
end:
    if(a->type.of == TYPE_NODE){
        mess->current = (Node *)a;
    }
    mess->currentValue = a;
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
    mess->root = mess->current = Node_Make(m, 0, NULL);
    return mess;
}
