#include <external.h>
#include <caneka.h>

gobits QueueCrit_Time(QueueCrit *crit, util *values){
    gobits go = 0;
    ApproxTime *dest = (ApproxTime *)&crit->u;
    ApproxTime *tp = (ApproxTime *)values;
    util base = 1;
    for(i32 i = 0; i < CRIT_SLAB_STRIDE; i++){
        ApproxTime *t = tp+i;
        if(t->value > 0 && t->value <= dest->value){
            util u = (base << i);
            go |= u;
        }
    }
    return go;
}
