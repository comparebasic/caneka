#include <external.h>
#include <caneka.h>

void *Json_From(MemCh *m, void *sv){
    Debug_Push(m, sv);
    StrVec *v = Ifc(m, sv, TYPE_STRVEC); 

    Roebling *rbl = JsonParser_Make(m, Cursor_Make(m, v));
    Debug_SetRef(m, rbl);

    Roebling_Run(rbl);
    Roebling_Last(rbl);
    if(rbl->nest == 0){
        Return(m, JsonParser_GetRoot(rbl));
    }else{
        void *ar[] = {
            v,
            rbl->dest,
            NULL
        };
        Error(m, FUNCNAME, FILENAME, LINENUMBER, "Error parsing json: $ -> @", ar);
        Return(m, NULL);
    }
}

void *Json_FromPath(MemCh *m, void *path){
    Debug_Push(m, path);
    Buff *bf = Buff_Make(m, ZERO);
    File_Open(bf, path, O_RDONLY);
    if(bf->type.state & ERROR){
        Return(m, NULL);
    }

    Buff_Read(bf);
    File_Close(bf);

    Roebling *rbl = JsonParser_Make(m, Cursor_Make(m, bf->v));
    Roebling_Run(rbl);
    Roebling_Last(rbl);
    if(rbl->nest == 0){
        Return(m, JsonParser_GetRoot(rbl));
    }else{
        void *ar[] = {
            path,
            JsonParser_GetRoot(rbl),
            NULL
        };
        Error(m, FUNCNAME, FILENAME, LINENUMBER, "Error parsing json: $ -> @", ar);
        Return(m, NULL);
    }
}
