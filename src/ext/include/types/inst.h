enum nodeobj_prop_idx {
    INST_PROPIDX_NAME = 0,
    INST_PROPIDX_ATTS = 1,
    INST_PROPIDX_CHILDREN = 2,
};

Inst *asInst(MemCh *m, void *a);
i32 Inst_Set(Span *inst, Table *seal, void *key, void *value);
Span *Inst_Make(MemCh *m, cls typeOf);
status Inst_ShowKeys(Buff *bf, Inst *inst, i32 indent);
void *Inst_ByPath(Span *inst, StrVec *path, void *value, word op, Span *coords);
#define Inst_GetByPath(n, p) Inst_ByPath((n), (p), NULL, SPAN_OP_GET, NULL)
#define Inst_SetByPath(n, p, v) Inst_ByPath((n), (p), (v), SPAN_OP_SET, NULL)
Table *Inst_GetTblOfAtt(Inst *nobj, void *key);
void *Inst_Att(Inst *nobj, void *key);
status Inst_SetAtt(Inst *nobj, void *key, void *value);
void *Inst_GetChild(Inst *nobj, void *key);
