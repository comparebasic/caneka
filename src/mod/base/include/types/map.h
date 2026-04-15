extern struct lookup *MapsLookup;

enum blanked_types {
    TYPE_BLANKED = (1 << 14),
};

typedef struct map {
    RangeType type;
    RangeType *atts;
    struct str **keys;
    struct span *tbl;
} Map;

Map *Map_Make(struct mem_ctx *m, i16 length, RangeType *atts, struct str **keys);
Map *Map_FromTable(struct mem_ctx *m, struct span *tbl, cls typeOf);
status Maps_Init(struct mem_ctx *m);
struct span *Map_ToTable(struct mem_ctx *m, void *_a);
Map *Map_Get(cls typeOf);
status Map_MakeTbl(struct mem_ctx *m, Map *map);
void *Map_FromOffset(struct mem_ctx *m, void *a, i16 offset, cls typeOf);
