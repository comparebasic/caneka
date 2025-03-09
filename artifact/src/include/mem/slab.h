/*
Slab
The slab element used by Span, the slab is almost entirely internal.
*/

#define Slab_setSlot(sl, idx, value) (memcpy((sl)+(idx), (value), sizeof(slot)))
#define Slab_Make(m) ((slab *)MemCtx_Alloc((m), sizeof(slab)))
#define Slab_valueAddr(sl, localIdx) ((sl)+(localIdx))
void **Slab_nextSlotPtr(slot *sl, i32 localIdx);
void *Slab_nextSlot(slot *sl, i32 localIdx);
