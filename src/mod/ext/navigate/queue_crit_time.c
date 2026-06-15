#include <external.h>
#include <caneka.h>

gobits QueueCrit_Time(QueueCrit *crit, util *values){
    gobits go = 0;
    ApproxTime *dest = (ApproxTime *)&crit->u;
    ApproxTime *tp = (ApproxTime *)values;
    util base = 1;
    for(i32 i = 0; i < CRIT_SLAB_STRIDE; i++){
        ApproxTime *t = tp+i;
        if(crit->type.state & DEBUG){
            void *ar[] = {
                t, 
                dest,
            };
            Out("^c.@ dest:@^0\n", ar);
        }
        if((t->type.state & UPPER_FLAGS) &&     
                (t->type.state & UPPER_FLAGS) <= (dest->type.state & UPPER_FLAGS) && 
                t->value <= dest->value){


            util u = (base << i);
            go |= u;
        }
    }
    return go;
}
