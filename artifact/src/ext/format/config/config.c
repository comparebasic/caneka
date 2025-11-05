#include <external.h>
#include <caneka.h>

Object *Config_FromVec(MemCh *m, StrVec *v){
    Cursor *curs = Cursor_Make(m, v);
    Roebling *rbl = FormatConfig_Make(m, curs, NULL);
    Roebling_Run(rbl);
    return FormatConfig_GetRoot(rbl);;
}

Object *Config_FromPath(MemCh *m, Str *path){
    Buff *bf = Buff_Make(m, ZERO);
    if(File_Exists(bf, path)){
        File_Open(bf, path, O_RDONLY);
        Buff_Read(bf);
        File_Close(bf);
        return Config_FromVec(m, bf->v);
    }
    return NULL;
}
