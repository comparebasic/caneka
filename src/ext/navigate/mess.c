#include <external.h>
#include <caneka.h>

status Mess_Outdent(Mess *mess){
    if(mess->current != NULL){
        mess->current = mess->current->parent; 
        Abstract *child = NULL;
        if(mess->current != NULL){
            child = mess->current->child;
        }
        mess->currentValue = child;
    }else{
        mess->currentValue = NULL;
    }
    if(mess->current != NULL){
        return SUCCESS;
    }
    return NOOP;
}

status Mess_Compare(MemCh *m, Mess *a, Mess *b){
    DebugStack_Push(a, a->type.of);
    Comp *comp = Comp_Make(m, (Abstract *)a->root, (Abstract *)b->root);
    if(a->type.state & DEBUG){
        comp->type.state |= DEBUG;
    }
    while((Compare(comp) & (SUCCESS|ERROR|NOOP)) == 0){}
    if(comp->type.state & DEBUG){
        Abstract *args[] = {
            (Abstract *)comp,
            NULL
        };
        Out("^p.End Compare @^0.\n", args);
    }

    DebugStack_Pop();
    return Compare(comp);
}

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

    if(tk->type.state & TOKEN_OUTDENT && mess->current->parent != NULL){
        Mess_Outdent(mess);
    }

    if(tk->typeOf == ZERO){
        if(tk->type.state & TOKEN_ATTR_VALUE){
            if(mess->nextAtts == NULL){
                mess->nextAtts = Table_Make(mess->m);
            }
            Table_Set(mess->nextAtts, (Abstract *)I16_Wrapped(mess->m, tk->captureKey),
                (Abstract *)a);
            DebugStack_Pop();
            return mess->type.state;
        }
    }else if(tk->typeOf == TYPE_NODE || tk->typeOf == TYPE_RELATION){
        nd = Node_Make(mess->m, 0, mess->current);
        memcpy(&nd->tk, tk, sizeof(Tokenize));
        nd->atts = mess->nextAtts; 
        mess->nextAtts = NULL;

        nd->captureKey = tk->captureKey;
        if(tk->typeOf == TYPE_RELATION){
            nd->typeOfChild = TYPE_RELATION;
            nd->child = (Abstract *)Relation_Make(mess->m, 0, NULL);
        }else{
            if(tk->type.state & TOKEN_ATTR_VALUE){
                Mess_AddAtt(mess, nd, 
                    (Abstract *)I16_Wrapped(mess->m, tk->captureKey), (Abstract *)a);
            }
        }
        a = (Abstract *)nd;
    }else if(tk->typeOf == TYPE_SPAN){
        a = (Abstract *)Span_Make(mess->m);
    }

    Mess_GetOrSet(mess, mess->current, a, tk);
    DebugStack_Pop();
    return mess->type.state;
}

status Mess_GetOrSet(Mess *mess, Node *node, Abstract *a, Tokenize *tk){
    Abstract *current = NULL;
    if(tk == NULL){
        memcpy(tk, &mess->current->tk, sizeof(Tokenize));
    }
    if(node == NULL || a == NULL){
        Abstract *args[] = {
            (Abstract *)a,
            (Abstract *)mess,
            NULL
        };
        Error(mess->m, FUNCNAME, FILENAME, LINENUMBER,
            "Node is NULL for abstract @, and mess @", args);
        return ERROR;
    }

    if(node->typeOfChild == TYPE_WRAPPED_PTR){
        a = (Abstract *)Ptr_Wrapped(mess->m, (Abstract *)a, tk->type.of);
    }

    current = a;
    if(tk->type.state & TOKEN_BY_TYPE){
        if(mess->current->captureKey != tk->captureKey && tk->typeOf != TYPE_NODE){
            Node *nd = Node_Make(mess->m, 0, mess->current);
            memcpy(&nd->tk, tk, sizeof(Tokenize));
            nd->atts = mess->nextAtts; 
            mess->nextAtts = NULL;

            nd->captureKey = tk->captureKey;
            Tokenize _tk;
            memset(&_tk, 0, sizeof(Tokenize));
            _tk.type.of = TYPE_TOKENIZE;
            Mess_GetOrSet(mess, node, (Abstract *)nd, &_tk);
            return Mess_GetOrSet(mess, mess->current, a, tk);
        }
    }

    if(tk->type.state & TOKEN_NO_CONTENT){
        if(tk->typeOf == TYPE_STRVEC){
            a = (Abstract *)StrVec_Make(mess->m);
            current = a;
        }else{
            current = NULL;
            goto end;
        }
    }

    if(mess->current != NULL && (mess->current->tk.type.state & TOKEN_INLINE)
            && ((tk->type.state & (TOKEN_NO_CONTENT|TOKEN_ATTR_VALUE)) == 0)){
        Mess_Outdent(mess);
        node = mess->current;
    }

    if(tk->type.state & TOKEN_ATTR_VALUE && 
            (tk->typeOf != ZERO && tk->typeOf != TYPE_NODE && tk->typeOf != TYPE_RELATION)){
        Mess_AddAtt(mess, mess->current, 
            (Abstract *)I16_Wrapped(mess->m, tk->captureKey), (Abstract *)a);
    }else if(node->typeOfChild == 0){
        node->child = a;
        node->typeOfChild = a->type.of;
        goto end;
    }else if(((tk->type.state & TOKEN_NO_COMBINE) == 0) &&
            mess->currentValue != NULL && CanCombine((Abstract *)mess->currentValue, a)){

        if((tk->type.state & TOKEN_SEPERATE) && 
                mess->currentValue->type.of == TYPE_STRVEC &&
                ((StrVec *)mess->currentValue)->total > 0){
            Str *s = Str_Ref(mess->m, (byte *)" ", 1, 2, STRING_COPY);
            Combine((Abstract *)mess->currentValue, (Abstract *)s);
        }
        Combine((Abstract *)mess->currentValue, a);
        current = NULL;
        goto end;
    }else if(node->typeOfChild == TYPE_SPAN){
        Span_Add((Span *)node->child, a);
        goto end;
    }else if(node->typeOfChild == TYPE_RELATION){
        Relation *rel = (Relation *)node->child;
        Relation_AddValue(rel, a);
        if(tk->type.state & LAST){
            if(rel->stride == 0){
                Relation_HeadFromValues(rel);
            }
        }
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
        Node *nd = (Node *)a;
        mess->current = nd;
        if(nd->typeOfChild == TYPE_RELATION){
            current = nd->child;
        }
    }
    if(current != NULL){
        mess->currentValue = current;
    }
    if(tk->captureKey != 0){
        mess->current->latestKey = tk->captureKey;
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
