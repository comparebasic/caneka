typedef struct ordered_table {
    Type type;
    MemCtx *m;
    /* Queue Idx */
	void *root;
    word flags;
    word count;
    quad  delayTicks;
    /* end QueuIdx */
    byte dims;
	int nvalues;
    int max_idx;
    struct {
        int get;
        int set;
        int selected;
    } metrics;
    /* end span */
    Span *order;
} OrdTable;

OrdTable *OrdTable_Make(MemCtx *m);
