#include <external.h>
#include <caneka.h>

static SpanDef span16Def;
static SpanDef tableDef;
static SpanDef span16x32mDef;
static SpanDef span4kx32mDef;
static SpanDef span4x16Def;
static SpanDef spanStringDef;

status SpanDef_Init(){
    /* span16 */
    memset(&span16Def, 0, sizeof(SpanDef));
    span16Def.typeOf = TYPE_SPAN;
    span16Def.stride = SPAN_DEFAULT_STRIDE;
    span16Def.idxSize = SPAN_DEFAULT_IDX_SIZE;
    span16Def.idxStride = SPAN_DEFAULT_STRIDE;
    span16Def.slotSize = SPAN_DEFAULT_SLOT_SIZE;
    span16Def.itemSize = SPAN_DEFAULT_ITEM_SIZE;

    /* span16 */
    memset(&tableDef, 0, sizeof(SpanDef));
    tableDef.typeOf = TYPE_TABLE;
    tableDef.stride = SPAN_DEFAULT_STRIDE;
    tableDef.idxSize = SPAN_DEFAULT_IDX_SIZE;
    tableDef.idxStride = SPAN_DEFAULT_STRIDE;
    tableDef.slotSize = sizeof(Hashed)/sizeof(void*);
    tableDef.itemSize = sizeof(Hashed);
    tableDef.flags = RAW;
    tableDef.dim_lookups[0] = 15;
    tableDef.dim_lookups[1] = 255;
    tableDef.dim_lookups[2] = 4095;
    tableDef.dim_lookups[3] = 0;
    tableDef.dim_lookups[4] = 0;
    tableDef.dim_lookups[5] = 0;
    tableDef.dim_lookups[6] = 0;
    tableDef.dim_lookups[7] = 0;

    /* span16x32m */
    memset(&span16x32mDef, 0, sizeof(SpanDef));
    span16x32mDef.typeOf = TYPE_QUEUE_SPAN;
    span16x32mDef.stride = 16;
    span16x32mDef.idxStride = 32;
    span16x32mDef.idxSize = 2;
    span16x32mDef.slotSize = SPAN_DEFAULT_SLOT_SIZE;
    span16x32mDef.itemSize = SPAN_DEFAULT_ITEM_SIZE;

    /*span4kx32m */
    memset(&span4kx32mDef, 0, sizeof(SpanDef));
    span4kx32mDef.typeOf = TYPE_SLAB_SPAN;
    span4kx32mDef.stride = 4096 / sizeof(void *);
    span4kx32mDef.idxStride = 32;
    span4kx32mDef.idxSize = 2;
    span4kx32mDef.slotSize = SPAN_DEFAULT_SLOT_SIZE;
    span4kx32mDef.itemSize = SPAN_DEFAULT_ITEM_SIZE;
    span4kx32mDef.valueHdr = SPAN_DEFAULT_HDR;

    /* span4x16 */
    memset(&span4x16Def, 0, sizeof(SpanDef));
    span4x16Def.typeOf = TYPE_MINI_SPAN;
    span4x16Def.stride = 4;
    span4x16Def.idxStride = 16;
    span4x16Def.idxSize = SPAN_DEFAULT_IDX_SIZE;
    span4x16Def.slotSize = SPAN_DEFAULT_SLOT_SIZE;
    span4x16Def.itemSize = SPAN_DEFAULT_ITEM_SIZE;
    span4x16Def.valueHdr = SPAN_DEFAULT_HDR;

    /* spanString */
    memset(&spanStringDef, 0, sizeof(SpanDef));
    spanStringDef.typeOf = TYPE_STRING_SPAN;
    spanStringDef.stride = STRING_CHUNK_SIZE / sizeof(void *);
    spanStringDef.idxStride = 16;
    spanStringDef.slotSize = SPAN_DEFAULT_SLOT_SIZE;
    spanStringDef.itemSize = SPAN_DEFAULT_ITEM_SIZE;
    spanStringDef.idxSize = SPAN_DEFAULT_IDX_SIZE;
    spanStringDef.valueHdr = sizeof(Type)+sizeof(int);

    return SUCCESS;
}

SpanDef *SpanDef_Clone(MemCtx *m, SpanDef *_def){
    SpanDef *def = (SpanDef *)MemCtx_Alloc(m, sizeof(SpanDef));
    memcpy(def, _def, sizeof(SpanDef));
    return def;
}

SpanDef *SpanDef_FromCls(word cls){
    if(cls == TYPE_SPAN){
        return &span16Def;
    }else if(cls == TYPE_MINI_SPAN){
        return &span4x16Def;
    }else if(cls == TYPE_TABLE){
        return &tableDef;
    }else if(cls == TYPE_QUEUE_SPAN){
        return &span16x32mDef;
    }else if(cls == TYPE_SLAB_SPAN){
        return &span4kx32mDef;
    }else if(cls == TYPE_STRING_SPAN){
        return &spanStringDef;
    }else{
        /* default to 16 x 16 */
        return &span16Def;
    }
}
