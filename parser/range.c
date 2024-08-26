#include <external.h>
#include <caneka.h>

status Range_Set(Range *range, String *s){
    memset(range, 0, sizeof(Range));
    range->type.of = TYPE_RANGE;
    range->search = range->start.s = range->start.seg = s;
    Range_Sync(range, &(range->start));
    return SUCCESS;
}

byte Range_GetByte(Range *range){
    if(range->potential.seg != NULL){
        return range->potential.seg->bytes[range->potential.position];
    }else{
        return '\0';
    }
}

status Range_Sync(Range *range, SCursor *cursor){
    if(&(range->start) != cursor){
        memcpy(&(range->start), cursor, sizeof(SCursor));
    }
    if(&(range->end) != cursor){
        memcpy(&(range->end), cursor, sizeof(SCursor));
    }
    if(&(range->potential) != cursor){
        memcpy(&(range->potential), cursor, sizeof(SCursor));
    }
    return SUCCESS;
}

int Range_GetLength(Range *range){
    if(range->end.position == -1){
        return 0;
    }
    String *seg = range->start.seg;
    int length = 0;
    if(seg == range->end.seg){
        length = range->end.position - range->start.position;
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

    return length;
}

String *Range_Copy(MemCtx *m, Range *range){
    String *seg = range->start.seg;
    String *s = NULL;
    int length = Range_GetLength(range);
    if(seg == range->end.seg){
        s = String_MakeFixed(m, seg->bytes + range->start.position, length); 
    }else{
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
