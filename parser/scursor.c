#include <external.h>
#include <caneka.h>

SCursor* SCursor_Make(MemCtx *m, String *s){
    SCursor *sc = (SCursor *) MemCtx_Alloc(m, sizeof(SCursor));
    sc->s = s;
    sc->seg = sc->s;

    return sc;
}

status SCursor_Reset(SCursor *sc){
    String *s = sc->s;
    memset(sc, 0, sizeof(SCursor));
    sc->s = s;
    sc->seg = s;
    return SUCCESS;
}

status SCursor_Find(Range *range, Match *search, Match *ko){
    SCursor *start = &(range->start); 
    SCursor *end = &(range->end); 
    if(start->seg == NULL || start->seg->length < 1){
        return NOOP;
    }
    Range_Reset(range, search->flags);
    byte c;
    int i = range->start.position;
    int startPosition = i;
    String *seg = start->seg;

    if((search->flags & ANCHOR_UNTIL) != 0){
        start->position = i;
        start->seg = seg;
        start->state = COMPLETE;
    }

    while(seg != NULL){
        for(
            c = seg->bytes[i];
            i < seg->length;
            ++i, c = seg->bytes[i]
        ){
            if(DEBUG_CURSOR){
                printf("\x1b[%dmMatch_Feed ->  i:%d - '%c' \x1b[0m\n", DEBUG_CURSOR, i, c);
            }
            Match_Feed(search, c);
            if(ko != NULL){
                Match_Feed(ko, c);
                if(ko->state == COMPLETE){
                    search->state = COMPLETE;
                    i -= (ko->position);
                }
            }
            if(DEBUG_CURSOR){
                Debug_Print((void *)search, 0, "", DEBUG_CURSOR, TRUE);
                printf("\n");
            }
            if(search->state != READY){
                if((search->flags & ANCHOR_UNTIL) != 0 && start->state == READY){
                    start->position = i;
                    start->seg = seg;
                    start->state = search->state;
                }
                range->compare++;
                start->state = PROCESSING;
                if(search->state == COMPLETE){
                    end->position = (i+1);
                    end->seg = seg;
                    range->length = -1;
                    return search->state;
                }else if((search->state & INVERTED) != 0){
                    search->state = COMPLETE;
                    end->position = i;
                    end->seg = seg;
                    range->length = -1;
                    return search->state;
                }
            }else{
                if((search->flags & ANCHOR_START) != 0){
                    start->state = READY;
                    return start->state;
                }else{
                    start->state = READY;
                    start->position = startPosition;
                }
            }

        }
        i = 0;
        seg = String_Next(seg);
    }

    if(DEBUG_ROEBLING){
        Debug_Print((void *)search, 0, "SCursor_Find of:", DEBUG_ROEBLING, TRUE);
        printf("\n");
    }

    return search->state;
}

status SCursor_Incr(SCursor *sc, i64 length){
    i64 remaining = length;
    i64 local;
    while(remaining > 0){
        local = sc->seg->length - sc->position;
        if(local > remaining){
            sc->position += remaining;
            remaining = 0;
            continue;
        }else{
            remaining -= local;
            sc->position += local;
            sc->seg = sc->seg->next;
        }
    }

    if(sc->seg == NULL && sc->position == String_Length(sc->s)){
        sc->state = COMPLETE;
        return sc->state;
    }

    return SUCCESS;
}
