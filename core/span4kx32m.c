#include <external.h>
#include <caneka.h>

static int stride = 4096 / sizeof(void *);
static int idxStride = 32;
static int idxExtraSlots = 1;
static int slotSize = SPAN_DEFAULT_SLOT_SIZE;
static int itemSize = SPAN_DEFAULT_ITEM_SIZE;

#include  <span_template.c>

SpanDef *Span4kx32m_MakeDef(){
    return spanDef_Make(); 
}
