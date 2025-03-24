#include <external.h>
#include <caneka.h>

#include "./mocks.c"
#include "../../base/sequence/span.c"
#include "../../base/sequence/iter.c"

int main(int argc, char *argv[]){
    printf("yay\n");
    Span *p = Span_Make(NULL);

    char *one = "ONE";
    Span_Set(p, 0, (Abstract *)one);
    printf("one: %s\n", (char *)Span_Get(p, 0));

    char *two = "TWO";
    Span_Set(p, 1, (Abstract *)two);
    printf("two: %s\n", (char *)Span_Get(p, 1));

    int idx = SPAN_STRIDE+1;
    printf("-- %d --\n", idx);
    p->type.state |= DEBUG;
    SpanQuery sq;
    SpanQuery_Setup(&sq, p, SPAN_OP_SET, idx);
    Span_Query(&sq);

    idx = 0;
    printf("-- %d --\n", idx);
    SpanQuery_Setup(&sq, p, SPAN_OP_SET, idx);
    Span_Query(&sq);

    idx = 7;
    printf("-- %d --\n", idx);
    SpanQuery_Setup(&sq, p, SPAN_OP_SET, idx);
    Span_Query(&sq);

    idx = 15;
    printf("-- %d --\n", idx);
    SpanQuery_Setup(&sq, p, SPAN_OP_SET, idx);
    Span_Query(&sq);
    
    idx = 16;
    printf("-- %d --\n", idx);
    SpanQuery_Setup(&sq, p, SPAN_OP_SET, idx);
    Span_Query(&sq);

    idx = 63;
    printf("-- %d --\n", idx);
    SpanQuery_Setup(&sq, p, SPAN_OP_SET, idx);
    Span_Query(&sq);

    idx = 64;
    printf("-- %d --\n", idx);
    SpanQuery_Setup(&sq, p, SPAN_OP_SET, idx);
    Span_Query(&sq);


    return 0;
}
