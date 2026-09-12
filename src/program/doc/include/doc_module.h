#ifndef DOC_MODULE_H
#define DOC_MODULE_H

typedef struct docinst {
    Type type;
    Buff *srcFile;
    Buff *outFile;
    Roebling *rbl;
} DocInst;

void Doc_GenFromFile(Node *ctx, DocInst *d, Buff *bf);
DocInst *DocInst_Make(MemCh *m, Node *ctx, StrVec *src, StrVec *out);
void Doc_GenNav(Node *ctx);
#endif
