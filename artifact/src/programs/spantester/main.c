#include <external.h>
#include <caneka.h>

#include "./mocks.c"
#include "../../base/mem/span.c"
#include "../../base/mem/iter.c"

int main(int argc, char *argv[]){
    printf("yay\n");
    Span *p = Span_Make(NULL);
    int idx = -1;

    char *one = "ONE";
    Span_Set(p, 0, (Abstract *)one);
    printf("one: %s\n", (char *)Span_Get(p, 0));

    char *two = "TWO";
    Span_Set(p, 1, (Abstract *)two);
    printf("two: %s\n", (char *)Span_Get(p, 1));

    p->type.state |= DEBUG;
    Iter it;
    Iter_Init(&it, p);

    idx = 7;
    printf("-- %d --\n", idx);
    Iter_Setup(&it, p, SPAN_OP_SET, idx);
    char *seven = "SEVEN";
    Span_SetFromQ(&it, (Abstract *)seven); 
    printf("seven: %s\n", (char *)Span_Get(p, idx));

    idx = 16;
    printf("-- %d --\n", idx);
    Iter_Setup(&it, p, SPAN_OP_SET, idx);
    char *sixteen = "SIXTEEN";
    Span_SetFromQ(&it, (Abstract *)sixteen); 
    printf("sixteen: %s\n", (char *)Span_Get(p, idx));

    idx = 63;
    printf("-- %d --\n", idx);
    Iter_Setup(&it, p, SPAN_OP_SET, idx);
    char *sixtyThree = "SIXTY_THREE";
    Span_SetFromQ(&it, (Abstract *)sixtyThree); 
    printf("sixy three: %s\n", (char *)Span_Get(p, idx));

    idx = 64;
    printf("-- %d --\n", idx);
    Iter_Setup(&it, p, SPAN_OP_SET, idx);
    char *sixtyFour = "SIXTY_FOUR";
    Span_SetFromQ(&it, (Abstract *)sixtyFour); 
    printf("sixty four: %s\n", (char *)Span_Get(p, idx));

    idx = 4073;
    printf("-- %d --\n", idx);
    Iter_Setup(&it, p, SPAN_OP_SET, idx);
    char *fourK73 = "Fourk73";
    Span_SetFromQ(&it, (Abstract *)fourK73); 
    printf("4073: %s\n", (char *)Span_Get(p, idx));

    idx = 70123;
    printf("-- %d --\n", idx);
    Iter_Setup(&it, p, SPAN_OP_SET, idx);
    char *seventyK123 = "70k123";
    Span_SetFromQ(&it, (Abstract *)seventyK123); 
    printf("70123: %s\n", (char *)Span_Get(p, idx));


    return 0;
}
