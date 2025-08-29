#define LICENCE_MAX 32
extern char *Licences[LICENCE_MAX];
extern char *libcnkext_licence_0;
extern char *libcnkext_version_0;

status Add_Licence(char *name, char *version, char *licence);
status Caneka_LicenceInit(MemCh *m);
status Show_Licences(Stream *sm);
