typedef struct filter {
    Abstract *seal;
    Span *filterBits;
    SourceFunc *filterFunc;
} Filter;
