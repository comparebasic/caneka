extern struct lookup *MapsLookup;

enum blanked_types {
    TYPE_BLANKED = (1 << 14),
};

typedef struct map {
    RangeType type;
    RangeType *atts;
    struct str **keys;
} Map;

Map *Map_Make(struct mem_ctx *m, i16 length, RangeType *atts, struct str **keys);
struct span *Map_ToTable(struct mem_ctx *m, Map *map);
Map *Map_FromTable(struct mem_ctx *m, struct span *tbl, cls typeOf);
status Maps_Init(struct mem_ctx *m);
