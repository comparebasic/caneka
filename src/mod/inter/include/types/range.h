enum inter_range {
    _TYPE_INTER_START = _TYPE_EXT_END,
    _TYPE_INTER_END,
};

enum inter_obj_range {
    _TYPE_INTER_OBJ_START = _NEXT_RANGE_START,
    _TYPE_INTER_OBJ_END,
};

#undef _NEXT_RANGE_START
#define _NEXT_RANGE_START _TYPE_INTER_END
