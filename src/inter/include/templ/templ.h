typedef struct templ {
    Type type;
    Type objType;
    MemCh *m;
    i32 level;
    Span *jumps;
    Span *funcs;
    Iter content;
    Iter data;
} Templ;

Templ *Templ_Make(MemCh *m, Span *content);
status Templ_Prepare(Templ *templ);
status Templ_Reset(Templ *templ);
status Templ_ToSCycle(Templ *templ, Buff *bf, void *source);
status Templ_ToS(Templ *templ, Buff *bf, void *data, void *source);
status Templ_SetData(Templ *templ, void *data);
Templ *Templ_ForFile(MemCh *m, StrVec *path);
