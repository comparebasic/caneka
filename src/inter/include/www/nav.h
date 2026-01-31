typedef Inst WwwNav;

WwwNav *WwwNav_Make(MemCh *m, StrVec *_path, StrVec *name);
status WwwNav_Add(WwwNav *root, WwwNav *item, Table *coordTbl);
status WwwNav_Init(MemCh *m);
