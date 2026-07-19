extern Str *FmtBlack;
extern Str *FmtRed;
extern Str *FmtYellow;
extern Str *FmtGreen;
extern Str *FmtPurple;
extern Str *FmtBlue;
extern Str *FmtCyan;
extern Str *FmtDark;

extern Str *AnsiBlack;
extern Str *AnsiRed;
extern Str *AnsiYellow;
extern Str *AnsiGreen;
extern Str *AnsiPurple;
extern Str *AnsiBlue;
extern Str *AnsiCyan;
extern Str *AnsiDark;

Str *Str_FromAnsi(Buff *bf, char **_ptr, char *end);
Str *Str_AnsiCstr(Buff *bf, char *cstr);
Str *Str_ConsumeAnsi(Buff *bf, char **_ptr, char *end, boolean consume);
status AnsiStr_Init(MemCh *m);
boolean Ansi_HasColor(Buff *bf);
void Ansi_SetColor(Buff *bf, boolean yn);
