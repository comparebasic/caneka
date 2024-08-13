#include <external.h>
#include <caneka.h>

/* all defaults */
static int stride = SPAN_DEFAULT_STRIDE;
static int idxStride = SPAN_DEFAULT_STRIDE;
static int idxExtraSlots = SPAN_DEFAULT_IDX_EXTRA_SLOTS;
static int slotSize = SPAN_DEFAULT_SLOT_SIZE;
static int itemSize = SPAN_DEFAULT_ITEM_SIZE;
static void *(*valueSlab_Make)(SlabResult *sr) = Span_valueSlab_Make;
static void *(*idxSlab_Make)(SlabResult *sr) = Span_idxSlab_Make;
static void *(*nextByIdx)(SlabResult *sr) = Span_nextByIdx;
static void *(*nextBySlot)(SlabResult *sr) = Span_nextBySlot;
static void *(*reserve)(SlabResult *sr) = Span_reserve;

#include  <span_template.c>

SpanDef *Span16_MakeDef(){
    return spanDef_Make(); 
}
