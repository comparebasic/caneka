status Templ_PrepareCycle(Templ *templ);
status Templ_Prepare(Templ *templ);
status Templ_AddJump(Templ *templ,
    i32 idx, i32 destIdx, i32 flagIdx, status flags);
TemplCrit *Templ_LastJumpAt(Templ *templ, i32 idx, i32 flagIdx);
