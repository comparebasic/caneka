typedef struct streamdef {
    Abstract *p; 
    word flags;
    ParseFunc complete;
} StreamDef;
