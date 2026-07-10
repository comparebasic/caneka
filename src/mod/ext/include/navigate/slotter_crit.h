/*(SPAN_STRIDE*SPAN_STRIDE)*/
#define CRIT_SLAB_STRIDE 64
/*(SPAN_STRIDE*SPAN_STRIDE)-1*/
#define CRIT_SLAB_MASK 63

enum slotter_crit_flags {
    SLOTTER_CRIT_REQUIRED = 1 << 11,
    SLOTTER_CRIT_UTIL = 1 << 12,
    SLOTTER_CRIT_PFD = 1 << 13,
};

typedef boolean (*SlotterFunc)(struct slotter_crit *crit, util *u);

typedef struct slotter_crit {
    Type type;
    util u;
    SlotterFunc func;
    Span *data;
} SlotterCrit;

SlotterCrit *SlotterCrit_Make(MemCh *m, SlotterFunc func, word flags);
status SlotterCrit_SetTick(SlotterCrit *crit, Slotter *q);
