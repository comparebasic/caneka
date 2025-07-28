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
    return Add_Licence("Caneka", libcaneka_licence_0);
}

status Show_Licences(Stream *sm){
    DebugStack_Push(NULL, ZERO);
    char **name = Licences;
    char **licence = name+1;
    status r = READY;
    while(*name != NULL){
        Abstract *args[] = {
            (Abstract *)Str_CstrRef(sm->m, *name),
            (Abstract *)Str_CstrRef(sm->m, *licence),
        };
        Fmt(sm, "$ LICENCE\n\n$\n\n", args);
        r |= SUCCESS;

        name += 2;
        licence = name+1;
    }

    if(r == READY){
        r |= NOOP;
    }
    DebugStack_Pop();
    return r;
}

status Add_Licence(char *name, char *licence){
    if(_count > LICENCE_MAX-3){
        Error(ErrStream->m, NULL, FILENAME, FUNCNAME, LINENUMBER, "Max Licences Reached", NULL);
        return ERROR;
    }
    Licences[_count++] = name;
    Licences[_count++] = licence;
    return SUCCESS;
}
