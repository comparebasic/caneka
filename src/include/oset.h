enum oset_states {
    OSET_START = 0,
    OSET_KEY = 0,
    OSET_LENGTH = 0,
    OSET_OPTS = 0,
    OSET_VALUE = 0
};


Roebling *OsetParser_Make(MemCtx *m, String *Source);
String *Oset_To(MemCtx *m, Abstract *a);
