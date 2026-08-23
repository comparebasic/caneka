typedef struct cmdfile {
    Type type;
    Roebling *rbl; 
    Buff *in;
    Buff *out;
    Iter it;
} CmdFile;

HandlerDef *CmdFile_DefMake(MemCh *m, Span *steps, Node *ext, Abstract *key, Node *config);
void *CmdFile_SourceMake(MemCh *m, Abstract *key, HandlerDef *def);
