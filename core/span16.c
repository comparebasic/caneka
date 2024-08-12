#include <external.h>
#include <caneka.h>


static int stride = SPAN_DEFAULT_STRIDE;
static int idxStride = SPAN_DEFAULT_STRIDE;
static int idxExtraSlots = SPAN_DEFAULT_IDX_EXTRA_SLOTS;
static int slotSize = SPAN_DEFAULT_SLOT_SIZE;
static int itemSize = SPAN_DEFAULT_ITEM_SIZE;

#include  <span_template.c>

SpanDef *Span16_MakeDef(){
    return spanDef_Make(); 
}
