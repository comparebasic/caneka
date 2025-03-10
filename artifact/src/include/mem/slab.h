/*
Slab
The slab element used by Span, the slab is almost entirely internal.
*/

#define Slab_Make(m) ((slab *)MemCtx_Alloc((m), sizeof(slab)))
#define Slab_GetSlot(sl, idx) (((void **)(sl))+(idx))
