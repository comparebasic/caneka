typedef struct templ {
    Type type;
    Type objType;
    MemCh *m;
    i32 level;
    struct {
        Span *p; /*<it>*/
        i32 idx;
        i32 incr;
    } indent;
    struct {
        Span *p; /*<it>*/
        i32 idx;
        i32 incr;
    } outdent;
    Iter content;
    Iter data;
} Templ;

Templ *Templ_Make(MemCh *m, Span *content);
status Templ_Prepare(Templ *templ);
status Templ_Reset(Templ *templ);
i64 Templ_ToSCycle(Templ *templ, Buff *bf, i64 total, void *source);
i64 Templ_ToS(Templ *templ, Buff *bf, void *data, void *source);
status Templ_SetData(Templ *templ, void *data);
