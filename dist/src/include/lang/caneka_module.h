enum cnk_module_flags {
    CNK_LANG_PRIVATE = 1 << 8,
    CNK_LANG_OPTIONAL = 1 << 9,
};

typedef struct caneka_statement {
    Type type;
    int id;
    String *name;
    Span *children;
} CnkLangStatement;

typedef struct caneka_func_def {
    Type type;
    String *name;
    Span *args;
} CnkLangFuncDef;

typedef struct caneka_module {
    Type type;
    String *name;
    String *hfile;
    String *cfile;
    String *typeName;
    String *typeId;
    Span *args;
    Span *funcDefs;
    struct caneka_module *parent;
} CnkLangModule;
