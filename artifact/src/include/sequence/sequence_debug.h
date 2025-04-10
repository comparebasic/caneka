i64 Iter_Print(Stream *sm, Abstract *a, cls type, boolean extended);
i64 Slab_Print(struct stream *sm, slab *slab, i8 dim, i8 dims);
i64 Span_Print(struct stream *sm, Abstract *a, cls type, boolean extended);
i64 Lookup_Print(struct stream *sm, Abstract *a, cls type, boolean extended);

status SequenceDebug_Init(MemCh *m, Lookup *lk);
