typedef Abstract *(*Maker)(struct mem_ctx *m, Abstract *a); /* mk */
Single *Maker_Wrapped(struct mem_ctx *m, Maker mk);
