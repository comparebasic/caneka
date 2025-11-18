#include <external.h>
#include <caneka.h>

gobits QueueCrit_Time(QueueCrit *crit, util *values){
    gobits go = 0;
    microTime dest = (microTime)crit->u;
    microTime *tp = (microTime *)values;
    util base = 1;
    for(i32 i = 0; i < CRIT_SLAB_STRIDE; i++){
        microTime t = tp[i];
        if(t == 0){
            printf("%d zero,", i);
        }else{
            printf("\x1b[1;33m%d non-zero\x1b[0m,", i);
        }
        fflush(stdout);
        if(t > 0 && t <= dest){
            util u = (base << i);
            void *args[] = {
                Str_Ref(ErrStream->m, (byte *)&u, sizeof(util), sizeof(util), STRING_BINARY),
                NULL
            };
            Out("^c. Adding @^0\n", args);
            go |= u;
        }
    }
    printf("\n");
    return go;
}
