#include <external.h>
#include <caneka.h>

static int stride = STRING_CHUNK_SIZE / sizeof(void *);
static int idxStride = 16;
static int valueHdr = sizeof(Type)+sizeof(int);
static word type = TYPE_STRING_SPAN;
/* defaults */
static int slotSize = SPAN_DEFAULT_SLOT_SIZE;
static int itemSize = SPAN_DEFAULT_ITEM_SIZE;
static int idxSize = SPAN_DEFAULT_IDX_SIZE;

#include  <span_template.c>

SpanDef *SpanString_MakeDef(){
    return spanDef_Make(); 
}
