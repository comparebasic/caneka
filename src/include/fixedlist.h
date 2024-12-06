typedef struct fixed_list {
    Type type;
    Queue *queue;
    Span *tbl; /* name to id in queue */
    File *file;
} FixedList;
