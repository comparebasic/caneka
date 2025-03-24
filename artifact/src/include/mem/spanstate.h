typedef struct span_state {
    i8 dim;
    i8 localIdx; 
    word _;
    i32 increment;
    i64 offset;
    void **ptr;
} SpanState;
