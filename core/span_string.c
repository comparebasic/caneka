#include <external.h>
#include <caneka.h>

static int stride = STRING_CHUNK_SIZE;
static int idxStride = 16;
static int idxExtraSlots = 0;
static int slotSize = SPAN_DEFAULT_SLOT_SIZE;
static int itemSize = SPAN_DEFAULT_ITEM_SIZE;

#include  <span_template.c>

SpanDef *SpanString_MakeDef(){
    return spanDef_Make(); 
}
