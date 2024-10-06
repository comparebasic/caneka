#include <external.h>
#include <caneka.h>

Handler *Handler_Get(Handler *h){
    /* if the root handler is finished, the request is complete */
    if((h->type.state & (SUCCESS|ERROR)) != 0){
        return h;
    }

    Handler *cursor = h;
    Handler *priorH = cursor;
    Span *recent = NULL;
    while(cursor != NULL && cursor->prior != NULL && (recent == NULL || !HasFlag(recent->type.state, SUCCESS))){
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
        return priorH;
    }

    return h;
}

Handler *Handler_Make(MemCtx *m, HandleFunc func, Abstract *data){
    Handler *h = MemCtx_Alloc(m, sizeof(Handler));
    h->func = func;
    h->data = data;
    return h;
}
