#include <external.h>
#include <caneka.h>

static i32 _count = 0;

char *Licences[] = {
    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
};

status Caneka_LicenceInit(MemCh *m){
    return Add_Licence("CANEKA", libcaneka_version_0, libcaneka_licence_0);
}

status Show_Licences(Stream *sm){
    DebugStack_Push(NULL, ZERO);
    char **name = Licences;
    char **version = name+1;
    char **licence = name+2;
    status r = READY;
    while(*name != NULL){
        Abstract *args[] = {
            (Abstract *)Str_CstrRef(sm->m, *name),
            (Abstract *)Str_CstrRef(sm->m, *version),
            (Abstract *)Str_CstrRef(sm->m, *licence),
        };
        Fmt(sm, "$ LICENCE (Software Version $) \n\n$\n\n", args);
        r |= SUCCESS;

        name += 3;
        version = name+1;
        licence = name+2;
    }

    if(r == READY){
        r |= NOOP;
    }
    DebugStack_Pop();
    return r;
}

status Add_Licence(char *name, char *version, char *licence){
    if(_count > LICENCE_MAX-4){
        Error(ErrStream->m, NULL, FILENAME, FUNCNAME, LINENUMBER, "Max Licences Reached", NULL);
        return ERROR;
    }
    Licences[_count++] = name;
    Licences[_count++] = version;
    Licences[_count++] = licence;
    return SUCCESS;
}
