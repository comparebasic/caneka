#include <external.h>
#include <caneka.h>

gobits QueueCrit_Time(QueueCrit *crit, util *values){
    word go = 0;
    microTime dest = (microTime)crit->u;
    microTime *tp = (microTime *)values;
    word base = 1;
    for(i32 i = 0; i < SPAN_STRIDE; i++){
        microTime t = *tp;
        if(t > 0 && t <= dest){
            go |= (base << i);
        }
        tp++;
    }
    return go;
}
