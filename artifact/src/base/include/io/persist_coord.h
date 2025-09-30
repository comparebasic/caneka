typedef struct ref_coords {
    cls typeOf;
    i16 idx;
    quad offset;
} PersistCoord;

void PersistCoord_Fill(PersistCoord *coord, i32 slIdx, void *ptr, cls typeOf);
