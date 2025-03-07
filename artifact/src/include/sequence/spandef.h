typedef struct span_def {
    word flags;
    word typeOf;
    int stride;
    int idxStride;
    int slotSize;
    int idxSize;
    int itemSize;
    int valueHdr;
    Maker valueSlab_Make;
    i64 dim_lookups[8];
} SpanDef;

SpanDef *SpanDef_FromCls(word cls);
SpanDef *SpanDef_Clone(MemCtx *m, SpanDef *_def);
status SpanDef_Init();
