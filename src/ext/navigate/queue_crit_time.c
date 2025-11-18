#include <external.h>
#include <caneka.h>

gobits QueueCrit_Time(QueueCrit *crit, util *values){
    gobits go = 0;
    microTime dest = (microTime)crit->u;
    microTime *tp = (microTime *)values;
    util base = 1;
    for(i32 i = 0; i < CRIT_SLAB_STRIDE; i++){
        microTime t = tp[i];
        if(t > 0 && t <= dest){
            util u = (base << i);
            go |= u;
        }
    }
    return go;
}
