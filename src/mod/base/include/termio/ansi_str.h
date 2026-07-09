extern Str *ansi_black;
extern Str *ansi_red;
extern Str *ansi_yellow;
extern Str *ansi_green;
extern Str *ansi_purple;
extern Str *ansi_blue;
extern Str *ansi_cyan;
extern Str *ansi_dark;

Str *Str_FromAnsi(Buff *bf, char **_ptr, char *end);
Str *Str_AnsiCstr(Buff *bf, char *cstr);
Str *Str_ConsumeAnsi(Buff *bf, char **_ptr, char *end, boolean consume);
status AnsiStr_Init(MemCh *m);
boolean Ansi_HasColor(Buff *bf);
void Ansi_SetColor(Buff *bf, boolean yn);
