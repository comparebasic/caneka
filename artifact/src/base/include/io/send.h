status Send_Add(SendRecv *sr, Str *s);
status Send_AddVec(SendRecv *sr, StrVec *v);
status Send_Send(SendRecv *sr);
status Send_AddSend(SendRecv *sr, Str *s);
status Send_SendFrom(SendRecv *sr, i32 fd, *offset);

SendRecv *Send_Make(MemCh *m, i32 fd, StrVec *v, word flags);
