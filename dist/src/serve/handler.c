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
    while(
            cursor != NULL && 
            cursor->prior != NULL && 
            (recent == NULL || (recent->type.state & SUCCESS) == 0)
        ){
        recent = cursor->prior;
        if(recent->metrics.selected == -1){
            recent->metrics.selected = 0;
        }
        priorH = cursor;
        cursor = Span_GetSelected(recent);
    }

    if((cursor->type.state & SUCCESS) != 0){
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

Handler *Handler_Make(MemCtx *m, HandleFunc func, Abstract *data, int direction, int id){
    Handler *h = MemCtx_Alloc(m, sizeof(Handler));
    h->type.of = TYPE_HANDLER;
    h->func = func;
    h->data = data;
    h->direction = direction;
    h->id = id;
    return h;
}
