#include <external.h>
#include <caneka.h>

static int stride = 16;
static int idxStride = 32;
static int idxSize = 2;
static int slotSize = SPAN_DEFAULT_SLOT_SIZE;
static int itemSize = SPAN_DEFAULT_ITEM_SIZE;
static int valueHdr = 0;
static word type = TYPE_QUEUE_SPAN;

#include  <span_template.c>

SpanDef *Span16x32m_MakeDef(){
    return spanDef_Make(); 
}
