#include <external.h>
#include <caneka.h>

status Range_Set(Range *range, String *s){
    memset(range, 0, sizeof(Range));
    range->type.of = TYPE_RANGE;
    range->search = range->start.s = range->start.seg = range->end.s = range->end.seg = s;
    return SUCCESS;
}

status Range_Incr(Range *range){
    range->start = range->end;
    return SUCCESS;
}

int Range_GetLength(Range *range){
    if(range->length >= 0){
        return range->length;
    }else{
        String *seg = range->start.seg;
        int length = 0;
        if(seg == range->end.seg){
            length =  range->end.position - range->start.position;
        }else{
            length = seg->length - range->start.position;
            seg = String_Next(seg);
            while(seg != NULL && seg != range->end.seg ){
                length += seg->length;
                seg = String_Next(seg);
            }
            if(seg != NULL){
                length += range->end.position;
            }
        }
        range->length = length;
        return range->length;
    }
}

String *Range_Copy(MemCtx *m, Range *range){
    String *seg = range->start.seg;
    String *s = NULL;
    int length = 0;
    if(seg == range->end.seg){
        length =  range->end.position - range->start.position;
        s = String_MakeFixed(m, seg->bytes + range->start.position, length); 
    }else{
        length = Range_GetLength(range);
        s = String_Init(m, length);
        String_AddBytes(m, s, seg->bytes + range->start.position, seg->length - range->start.position); 
        seg = String_Next(seg);
        while(seg != NULL && seg != range->end.seg ){
            String_Add(m, s, seg); 
            seg = String_Next(seg);
        }
        if(seg != NULL){
            String_AddBytes(m, s, seg->bytes, range->end.position); 
        }
    }

    return s;
}

status Range_Reset(Range *range, int anchor){
    range->compare = 0;
    if(range->state == COMPLETE){
        range->start.position = range->end.position;
        range->start.seg = range->end.seg;
        if(range->start.position == range->start.seg->length){
            range->start.seg = range->start.seg->next;
        }
        range->start.position++;
    }

    range->state = READY;
    if(anchor == ANCHOR_START){
        range->start.state = READY;
    }else{
        range->start.state = PROCESSING;
    }

    return range->state;
}

