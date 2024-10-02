typedef Abstract *(*Maker)(struct mem_handle *m, Abstract *a); /* mk */
Single *Maker_Wrapped(MemHandle *m, Maker mk);
