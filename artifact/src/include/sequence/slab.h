/*
Slab
The slab element used by Span, the slab is almost entirely internal.
*/

#define SLOT_BYTE_SIZE (sizeof(void *))

status Slab_setSlot(void *sl, int idx, byte slotSize, byte ptrSlot, void *value);
void **Slab_nextSlotPtr(void *sl, int localIdx, byte slotSize, byte ptrSlot);
void *Slab_nextSlot(void *sl, int localIdx, byte slotSize, byte ptrSlot);
void *Slab_valueAddr(void *sl, int localIdx, byte slotSize, byte ptrSlot);
void *Slab_Make(MemCtx *m);
