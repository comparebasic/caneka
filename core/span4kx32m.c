#include <external.h>
#include <caneka.h>

static int stride = 4096 / sizeof(void *);
static int idxStride = 32;
static int idxExtraSlots = 1;

/* defaults */
static int slotSize = SPAN_DEFAULT_SLOT_SIZE;
static int itemSize = SPAN_DEFAULT_ITEM_SIZE;
static void *(*valueSlab_Make)(MemCtx *m, SpanDef *def) = Span_valueSlab_Make;
static void *(*idxSlab_Make)(MemCtx *m, SpanDef *def) = Span_idxSlab_Make;
static void *(*nextByIdx)(SlabResult *sr) = Span_nextByIdx;
static void *(*nextBySlot)(SlabResult *sr) = Span_nextBySlot;
static void *(*reserve)(SlabResult *sr) = Span_reserve;

#include  <span_template.c>

SpanDef *Span4kx32m_MakeDef(){
    return spanDef_Make(); 
}
