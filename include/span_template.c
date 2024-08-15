static SpanDef def;
static boolean _init = FALSE;

static SpanDef *spanDef_Make(){
    if(!_init){
        def.stride = stride;
        def.idxStride = idxStride;
        def.slotSize = slotSize;
        def.itemSize = itemSize;
        def.idxExtraSlots = idxExtraSlots;
        def.valueHdr = valueHdr;

        _init = TRUE;
    }
    return &def;
}
