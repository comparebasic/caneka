#include <external.h>
#include <caneka.h>

Inst *Config_FromVec(MemCh *m, StrVec *v){
    Cursor *curs = Cursor_Make(m, v);
    Roebling *rbl = FormatConfig_Make(m, curs, NULL);
    Roebling_Run(rbl);
    return FormatConfig_GetRoot(rbl);
}

Span *Config_Sequence(MemCh *m, StrVec *v){
    PatCharDef def[] = {
        {PAT_MANY|PAT_TERM, ' ', ' '},
        {PAT_END, 0, 0}
    };
    Match *mt = Match_Make(m, (PatCharDef *)&def, Span_Make(m));
    mt->type.state |= MATCH_SEARCH;
    StrVec *_v = StrVec_Make(m);
    StrVec_Split(v, mt);
    return v->p;
}

Inst *Config_FromPath(MemCh *m, Str *path){
    Buff *bf = Buff_Make(m, ZERO);
    if(File_Exists(bf, path)){
        File_Open(bf, path, O_RDONLY);
        Buff_Read(bf);
        File_Close(bf);
        return Config_FromVec(m, bf->v);
    }
    return NULL;
}
