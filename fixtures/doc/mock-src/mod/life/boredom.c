/* Life.Bordeom
 *
 * The toxic sludge that smolders and burns is fuled by BorEDoom.
 */
#include <external.h>
#include <caneka.h>

void *Boredom_GetIntoTrouble(MemCh *m, i64 random){
    if(random > 1000){
        return NationalDisaster_Make(m);
    }else{
        return NULL;
    }
}
