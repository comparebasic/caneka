#include <external.h>
#include <caneka.h>

/* all defaults */
static int stride = SPAN_DEFAULT_STRIDE;
static int idxSize = SPAN_DEFAULT_IDX_SIZE;
static int idxStride = SPAN_DEFAULT_STRIDE;
static int slotSize = SPAN_DEFAULT_SLOT_SIZE;
static int itemSize = SPAN_DEFAULT_ITEM_SIZE;
static int valueHdr = 0;
static word type = TYPE_SPAN;

#include  <span_template.c>

SpanDef *Span16_MakeDef(){
    return spanDef_Make(); 
}
