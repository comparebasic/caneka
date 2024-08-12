#include <external.h>
#include <caneka.h>

static int stride = 4096 / sizeof(void *);
static int idxStride = 32;

#include  <span_template.c>

SpanDef *Span16_MakeDef(){
    return spanDef_Make(); 
}
