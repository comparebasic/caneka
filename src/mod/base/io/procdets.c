/* Base.io.ProcDets
 *
 * Process information for managing a sub-process
 */

#include <external.h>
#include "base_module.h"

status ProcDets_Init(MemCh *m, ProcDets *pd){
    memset(pd, 0, sizeof(ProcDets));
    pd->type.of = TYPE_PROCDETS;
    pd->code = -1;
    pd->m = m;
    return SUCCESS;
}
