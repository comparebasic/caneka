typedef struct mhabstract  {
    Type type;
    struct mem_ctx *m;
    Abstract *a;
} MHAbstract;

status MHAbstract_Init(MHAbstract *ma, struct mem_ctx *m, Abstract *a);
