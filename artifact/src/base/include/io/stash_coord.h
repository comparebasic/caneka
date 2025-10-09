typedef struct ref_coords {
    cls typeOf;
    i16 idx;
    quad offset;
} StashCoord;

void StashCoord_Fill(StashCoord *coord, i32 slIdx, void *ptr, cls typeOf);
