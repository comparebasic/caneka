typedef struct span_state {
    i8 dim;
    i8 localIdx; 
    i16 increment;
    i32 offset;
    slab *slab;
} SpanState;
