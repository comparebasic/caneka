status Send_Add(Buff *bf, Str *s);
status Send_AddVec(Buff *bf, StrVec *v);
status Send_Unbuff(Buff *bf, byte *bytes, word length);
status Send_Send(Buff *bf);
status Send_AddSend(Buff *bf, Str *s);
status Send_SendAll(Buff *bf, StrVec *v);
