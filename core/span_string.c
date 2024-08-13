#include <external.h>
#include <caneka.h>

static int stride = STRING_CHUNK_SIZE / sizeof(void *);
static int idxStride = 16;
static int idxExtraSlots = 0;
/* defaults */
static int slotSize = SPAN_DEFAULT_SLOT_SIZE;
static int itemSize = SPAN_DEFAULT_ITEM_SIZE;

static void *valueSlab_Make(MemCtx *m, SpanDef *def){
    return (void *)(String_Init(m, STRING_EXTEND));
}

/* default functions */
static void *(*idxSlab_Make)(MemCtx *m, SpanDef *def) = Span_idxSlab_Make;
static void *(*nextByIdx)(SlabResult *sr) = Span_nextByIdx;
static void *(*nextBySlot)(SlabResult *sr) = Span_nextBySlot;
static void *(*reserve)(SlabResult *sr) = Span_reserve;

#include  <span_template.c>

SpanDef *SpanString_MakeDef(){
    return spanDef_Make(); 
}
