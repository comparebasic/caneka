static SpanDef def;
static boolean _init = FALSE;

static SpanDef *spanDef_Make(){
    if(!_init){
        memset(&def, 0, sizeof(SpanDef));
        def.typeOf = type;
        def.stride = stride;
        def.idxStride = idxStride;
        def.slotSize = slotSize;
        def.itemSize = itemSize;
        def.idxSize = idxSize;
        def.valueHdr = valueHdr;

        _init = TRUE;
    }
    return &def;
}
