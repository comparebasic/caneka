#include <external.h>
#include <caneka.h>

QueueCrit *QueueCrit_Make(MemCh *m, QueueFunc func, word flags){
    QueueCrit *crit = MemCh_AllocOf(m, sizeof(QueueCrit), TYPE_QUEUE_CRIT);
    crit->type.of = TYPE_QUEUE_CRIT;
    crit->type.state = flags;
    crit->func = func;
    crit->data = Span_Make(m);
    return crit;
}
