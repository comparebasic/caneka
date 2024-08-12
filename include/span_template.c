static SpanDef def;
static boolean _init = FALSE;

#ifndef stride
    static int stride = SPAN_DEFAULT_STRIDE;
#endif
#ifndef idxStride
    static int idxStride = SPAN_DEFAULT_STRIDE;
#endif
#ifndef slotsSize
    static int slotSize = SPAN_DEFAULT_SLOT_SIZE;
#endif
#ifndef itemSize
    static int itemSize = SPAN_DEFAULT_ITEM_SIZE;
#endif
#ifndef idxExtraSlots 
    static int idxExtraSlots = SPAN_DEFAULT_IDX_EXTRA_SLOTS;
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
