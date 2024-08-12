
#include <external.h>
#include <caneka.h>

static int stride = 16;
static int idxStride = 32;
static int idxExtraSlots = 1;

#include  <span_template.c>

SpanDef *Span16_MakeDef(){
    return spanDef_Make(); 
}
