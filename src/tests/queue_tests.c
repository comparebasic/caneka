#include <external.h>
#include <caneka.h>
#include <testsuite.h>

status Queue_Tests(MemCtx *gm){
    status r = SUCCESS;
    MemCtx *m = MemCtx_Make();

    Span *p = Span_Make(m, TYPE_QUEUE_SPAN);
    String *zero = String_Make(m, bytes("zero"));
    String *one = String_Make(m, bytes("one"));
    String *two = String_Make(m, bytes("two"));
    String *six = String_Make(m, bytes("six"));
    String *thirtySeven = String_Make(m, bytes("thirty-seven"));
    String *sevenHundred = String_Make(m, bytes("seven-hundred"));
    printf("\n");
    printf("\n");

    Span_Set(p, 0, (Abstract *)zero);
    Span_Set(p, 1, (Abstract *)one);
    Span_Set(p, 2, (Abstract *)two);
    Span_Set(p, 6, (Abstract *)six);
    Span_Set(p, 37, (Abstract *)thirtySeven);
    Queue_SetFlags(SLAB_ACTIVE, 0, 1000);
    Span_Set(p, 700, (Abstract *)sevenHundred);
    Queue_SetFlags(SLAB_FULL, 0, 3000);

    Debug_Print((void *)p, 0, "Queue: ", COLOR_PURPLE, TRUE);
    
    return r;
}
