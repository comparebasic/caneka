#define DIM_MAX 8 /* max dim size of 8 is 8^16 = 4,294,967,296 aka 4Billion */

enum span_flags {
    SPAN_RAW = 1 << 8, /* non Abstract data items such as i8, i64 or func* */
    SPAN_SORTED = 1 << 9, /* ordered according to a type comparison Func */
    SPAN_QUEUED = 1 << 10, /* re-uses indexes in a hotel style way */
    SPAN_DICT = 1 << 11, /* non-dim related Slates of Slates by collision */
};

typedef struct span {
    Type type;
    RangeType range; /* type/dims */
    struct mem_ctx *m;
    i64 size;
    i64 maxIdx;
    struct slate *root;
} Span;

void *Span_Get(Span *p, i64 idx);
i64 Span_Set(Span *p, i64 idx, void *t);
i64 Span_Remove(Span *p, i64 idx);
i64 Span_Add(Span *p, void *t);

util Span_SetSlot(Span *p, i64 idx, util u);
util Span_GetSlot(Span *p, i64 idx);

void Span_Init(struct mem_ctx *m, Span *p, field16 flags, struct slate *root);

Span *Span_Make(struct mem_ctx *m, field16 flags);
