#include <external.h>
#include <caneka.h>

static int stride = 4;
static int idxStride = 16;
static int idxExtraSlots = SPAN_DEFAULT_IDX_EXTRA_SLOTS;
static int slotSize = SPAN_DEFAULT_SLOT_SIZE;
static int itemSize = SPAN_DEFAULT_ITEM_SIZE;
static int valueHdr = 0;

#include  <span_template.c>

SpanDef *Span4x16_MakeDef(){
    return spanDef_Make(); 
}
