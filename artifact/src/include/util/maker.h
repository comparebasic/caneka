typedef Abstract *(*Maker)(struct span *m, Abstract *a); /* mk */
typedef Abstract *(*FlagMaker)(struct span *m, Abstract *a, word flags); /* fmk */
Single *Maker_Wrapped(struct span *m, Maker mk);
