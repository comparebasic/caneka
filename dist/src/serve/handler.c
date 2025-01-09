#include <external.h>
#include <caneka.h>
Handler *Handler_Current(Handler *h){
    if((h->type.state & (SUCCESS|ERROR)) != 0){
        return h;
    }

    Handler *cursor = h;
    while(
            h != NULL && 
            h->prior != NULL && 
            (h->prior== NULL || !HasFlag(h->prior->type.state, SUCCESS))
        ){
        h = Span_GetSelected(h->prior);
    }
    return h;
}

Handler *Handler_Get(Handler *h){
    /* if the root handler is finished, the request is complete */
    if((h->type.state & (SUCCESS|ERROR)) != 0){
        if(DEBUG_EXAMPLE_HANDLERS){
            printf("\x1b[%dm   returning same handler\x1b[0m\n", DEBUG_EXAMPLE_HANDLERS);
        }
        return h;
    }

    if(DEBUG_EXAMPLE_HANDLERS){
        printf("\x1b[%dm   returning new handler\x1b[0m\n", DEBUG_EXAMPLE_HANDLERS);
    }

    Handler *cursor = h;
    Handler *priorH = cursor;
    Span *recent = NULL;
    while(
            cursor != NULL && 
            cursor->prior != NULL && 
            (recent == NULL || !HasFlag(recent->type.state, SUCCESS))
        ){
        recent = cursor->prior;
        if(recent->metrics.selected == -1){
            recent->metrics.selected = 0;
        }
        priorH = cursor;
        cursor = Span_GetSelected(recent);
    }

    if(HasFlag(cursor->type.state, SUCCESS)){
        recent->metrics.selected++;  
        if(recent->metrics.selected > recent->max_idx){
            recent->type.state |= SUCCESS;
        }
        return Span_GetSelected(recent);
    }

    return cursor;
}

status Handler_AddPrior(MemCtx *m, Handler *orig, Handler *h){
    if(orig->prior == NULL){
        orig->prior = Span_Make(m, TYPE_SPAN);
    }
    return Span_Add(orig->prior, (Abstract *)h);
}

Handler *Handler_Make(MemCtx *m, HandleFunc func, Abstract *data, int direction){
    Handler *h = MemCtx_Alloc(m, sizeof(Handler));
    h->func = func;
    h->data = data;
    h->direction = direction;
    return h;
}
