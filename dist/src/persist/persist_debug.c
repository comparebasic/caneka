#include <external.h>
#include <caneka.h>
void ProcDets_Print(Abstract *a, cls type, char *msg, int color, boolean extended){
    ProcDets *pd = (ProcDets *)as(a, TYPE_PROCDETS);
    printf("\x1b[%dmProcDets<%s %d return:%d>\x1b[0m", color,
        State_ToChars(pd->type.state), pd->pid, pd->code);
}
