static SpanDef def;
static boolean _init = FALSE;

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
