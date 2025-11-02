#include <external.h>
#include <caneka.h>

gobits QueueCrit_Time(QueueCrit *crit, util *values){
    word go = 0;
    microTime dest = (microTime)crit->u;
    microTime *t = (microTime *)values;
    word base = 1;
    for(i32 i = 0; i < SPAN_STRIDE; i++){
        if(*t >= 0 && *t >= dest){
            go |= (base << i);
        }
        t++;
    }
    return go;
}
