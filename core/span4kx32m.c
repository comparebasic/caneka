#include <external.h>
#include <caneka.h>

static int stride = 4096 / sizeof(void *);
static int idxStride = 32;
static int idxSize = 2;
static int type = TYPE_SLAB_SPAN;

/* defaults */
static int slotSize = SPAN_DEFAULT_SLOT_SIZE;
static int itemSize = SPAN_DEFAULT_ITEM_SIZE;
static int valueHdr = SPAN_DEFAULT_HDR;

#include  <span_template.c>

SpanDef *Span4kx32m_MakeDef(){
    return spanDef_Make(); 
}
