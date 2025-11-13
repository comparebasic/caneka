Task *WebServer_Make(i32 port, quad ip4, util *ip6);
status WebServer_ServePage(Step *st, Task *tsk);
status WebServer_AddRoute(MemCh *m, Route *pages, StrVec *dir, StrVec *path);
status WebServer_LayRoute(MemCh *m,
    Route *pages, StrVec *dir, StrVec *path, boolean overlay);
status WebServer_GatherPage(Step *st, Task *tsk);
