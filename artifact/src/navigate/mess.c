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
    Abstract *a = (Abstract *)v;
    Node *nd = NULL;
    if(tk->type.state & TOKEN_CLOSE_OUTDENT){
        if(mess->currentValue != NULL && mess->currentValue != mess->current->child){
            mess->currentValue = mess->current->child;
        }else if(mess->current->parent != NULL){
            mess->current = mess->current->parent; 
            mess->currentValue = mess->current->child;
        }
        return mess->type.state;
    }
    if(tk->typeOf == TYPE_NODE){
        nd = Node_Make(mess->m, 0, mess->current);
        nd->captureKey = tk->captureKey;
        a = (Abstract *)nd;
        if(tk->type.state & TOKEN_ATTR_VALUE){
            Mess_AddAtt(mess, nd, 
                (Abstract *)I16_Wrapped(mess->m, tk->captureKey), (Abstract *)v);
        }
    }
    Mess_GetOrSet(mess, mess->current, a, tk);
    DebugStack_Pop();
    return mess->type.state;
}

status Mess_GetOrSet(Mess *mess, Node *node, Abstract *a, Tokenize *tk){
    if(node == NULL || a == NULL){
        Abstract *args[] = {
            (Abstract *)a,
            (Abstract *)mess,
            NULL
        };
        Error(mess->m, (Abstract *)mess, FUNCNAME, FILENAME, LINENUMBER,
            "Node is NULL for abstract @, and mess @", args);
        return ERROR;
    }

    Abstract *current = a;
    if(node->typeOfChild == TYPE_WRAPPED_PTR){
        a = (Abstract *)Ptr_Wrapped(mess->m, (Abstract *)a, tk->type.of);
    }

    if(tk != NULL && (tk->type.state & TOKEN_NO_COMBINE)){
        if(mess->current->captureKey != tk->captureKey){
            Node *nd = Node_Make(mess->m, 0, mess->current);
            nd->captureKey = tk->captureKey;
            Mess_GetOrSet(mess, node, (Abstract *)nd, NULL);
            return Mess_GetOrSet(mess, mess->current, a, tk);
        }
    }

    if(node->typeOfChild == 0){
        node->child = a;
        node->typeOfChild = a->type.of;
        goto end;
    }else if((tk == NULL || (tk->type.state & TOKEN_NO_COMBINE) == 0) &&
            Combine((Abstract *)mess->currentValue, a)){
        Abstract *args[] = {
            (Abstract *)a,
            (Abstract *)Type_ToStr(mess->m, mess->currentValue->type.of),
            (Abstract *)mess->currentValue,
            NULL
        };
        current = NULL;
        Out("Combining @ into $/@\n", args);
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
    if(current != NULL){
        mess->currentValue = current;
    }
    return node->type.state;
}

status Mess_Append(Mess *mess, Node *node, Abstract *a){
    if(a->type.of == TYPE_MESS){
        Node *child = (Node *)a;
        child->parent = node;
    }
    return Mess_GetOrSet(mess, node, a, NULL);
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
