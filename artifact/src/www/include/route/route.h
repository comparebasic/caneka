typedef Object Route;

Route *Route_Make(MemCh *m);
status Route_Collect(Route *rt, StrVec *path);
status Route_ClsInit(MemCh *m);
status Route_SetTargetFile(Route *rt, Str *ext, Str *absPath);
