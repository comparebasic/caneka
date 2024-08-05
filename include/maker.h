typedef Abstract *(*Maker)(struct mem_ctx *m, Abstract *a); /* mk */
Single *Maker_Wrapped(MemCtx *m, Maker mk);
