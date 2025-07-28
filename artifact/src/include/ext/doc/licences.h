#define LICENCE_MAX 32
extern char *Licences[LICENCE_MAX];
extern char *libcaneka_licence_0;

status Add_Licence(char *name, char *licence);
status Caneka_LicenceInit(MemCh *m);
status Show_Licences(Stream *sm);
