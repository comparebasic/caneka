typedef Abstract *(*Maker)(struct mem_ctx *m, Abstract *v);
Abstract *Maker_Make(struct mem_ctx *m, void *mk, cls type);

typedef struct maker {
    Type type;;
    Maker *mk;
} MakerObj;
