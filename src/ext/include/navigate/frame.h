typedef struct nested_frame {
    Type type;
    i32 originIdx;
    Abstract *originKey;
    Abstract *value;
    Iter it;
} Frame;

Frame *Frame_Make(MemCh *m, i32 originIdx, Abstract *originKey, Abstract *value, Span *p);
