typedef Abstract *(*Maker)(struct mem_ctx *m, Abstract *a); /* mk */
typedef Abstract *(*FlagMaker)(struct mem_ctx *m, Abstract *a, word flags); /* fmk */
Single *Maker_Wrapped(struct mem_ctx *m, Maker mk);
