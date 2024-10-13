/*
Slab
The slab element used by Span, the slab is almost entirely internal.

related: core/slab.c
related: include/span.h
*/

status Slab_setSlot(void *sl, struct span_def *def, int idx, void *value, size_t sz);
void *Slab_nextSlot(void *sl, struct span_def *def, int local_idx);
void **Slab_nextSlotPtr(void *sl, struct span_def *def, int local_idx);
void *Slab_valueAddr(void *sl, struct span_def *def, int local_idx);
