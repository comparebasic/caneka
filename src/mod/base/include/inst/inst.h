typedef struct span Inst;

enum nodeobj_prop_idx {
    INST_PROPIDX_NAME = 0,
    INST_PROPIDX_ATTS = 1,
    INST_PROPIDX_CHILDREN = 2,
};

Inst *asInst(struct mem_ctx *m, void *a);
struct span *IoPath_ToInstPath(struct mem_ctx *m, struct strvec *path);
i32 Inst_Set(struct span *inst, struct span *seal, void *key, void *value);
struct span *Inst_Make(struct mem_ctx *m, cls typeOf);
status Inst_ShowKeys(struct buff *bf, Inst *inst, i32 indent);
void *Inst_ByPath(struct span *inst, struct span *path, void *value, word op, struct span *coords);
#define Inst_GetByPath(n, p) Inst_ByPath((n), (p), NULL, SPAN_OP_GET, NULL)
#define Inst_SetByPath(n, p, v) Inst_ByPath((n), (p), (v), SPAN_OP_SET, NULL)
struct span *Inst_GetTblOfAtt(Inst *inst, void *key);
void *Inst_Att(Inst *inst, void *key);
status Inst_SetAtt(Inst *inst, void *key, void *value);
void *Inst_GetChild(Inst *inst, void *key);
i32 Inst_SetChild(Inst *inst, void *key, void *value);
void *Inst_GetNthChild(Inst *inst, i32 nth);
void Inst_ChAttsAdd(Inst *inst, void *att, struct span *tbl);
void Inst_IterInitChild(struct iter *it, Inst *inst, struct span *path);
