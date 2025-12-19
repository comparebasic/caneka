#define PASSWORD_UNIT_LENGTH 512

status Password_OnStr(MemCh *m, Str *dest, Str *pw, Str *salt);
status Password_Set(Buff *bf, Str *pw, Str *salt);
status Password_Check(Buff *bf, Str *pw, Str *salt);
