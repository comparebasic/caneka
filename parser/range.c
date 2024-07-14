#include <external.h>
#include <caneka.h>

status Range_Set(Range *range, String *s){
    memset(range, 0, sizeof(Range));
    range->type.of = TYPE_RANGE;
    range->search = range->start.s =  range->start.seg = range->end.s = range->end.seg = s;
    return SUCCESS;
}

status Range_Reset(Range *range, int anchor){
    range->compare = 0;
    if(range->state == COMPLETE){
        range->start.position = range->end.position;
        range->start.seg = range->end.seg;
        if(range->start.localPosition == range->start.seg->length){
            range->start.seg = range->start.seg->next;
        }
        range->start.position++;
        SCursor_SetLocals(&(range->start));
    }

    range->state = READY;
    if(anchor == ANCHOR_START){
        range->start.state = READY;
    }else{
        range->start.state = PROCESSING;
    }

    return range->state;
}

