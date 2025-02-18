typedef struct ordered_table {
    Type type;
    MemCtx *m;
    SpanDef *def;
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
} OrderedTable;

OrderedTable *OrderedTable_Make(MemCtx *m);
