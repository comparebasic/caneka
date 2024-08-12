static SpanDef def;
static boolean _init = FALSE;

#ifndef valueSlab_Make
    static void *(*valueSlab_Make)(SlabResult *sr) = Span_valueSlab_Make;
#endif
#ifndef idxSlab_Make
    static void *(*idxSlab_Make)(SlabResult *sr) = Span_idxSlab_Make;
#endif
#ifndef nextByIdx
    static void *(*nextByIdx)(SlabResult *sr) = Span_nextByIdx;
#endif
#ifndef nextBySlot
    static void *(*nextBySlot)(SlabResult *sr) = Span_nextBySlot;
#endif
#ifndef reserve
    static void *(*reserve)(SlabResult *sr) = Span_reserve;
#endif

static SpanDef *spanDef_Make(){
    if(!_init){
        def.stride = stride;
        def.idxStride = idxStride;
        def.slotSize = slotSize;
        def.itemSize = itemSize;
        def.idxExtraSlots = idxExtraSlots;

        def.valueSlab_Make = valueSlab_Make;
        def.idxSlab_Make = idxSlab_Make;
        def.nextByIdx = nextByIdx;
        def.nextBySlot = nextBySlot;
        def.reserve = reserve;
        _init = TRUE;
    }
    return &def;
}
