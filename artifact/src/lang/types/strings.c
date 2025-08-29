#include <external.h>
#include <caneka.h>

static char *langTypeStrings[] = {
    "_TYPE_LANG_START",
    "TYPE_TEMPL",
    "TYPE_TEMPL_CTX",
    "TYPE_TEMPL_JUMP",
    "_TEMPL_START",
    "TEMPL_MARK_START",
    "TEMPL_BETWEEN",
    "TEMPL_VALUE",
    "TEMPL_NOOP",
    "FORMAT_TEMPL_TEXT",
    "FORMAT_TEMPL_TEMPL",
    "FORMAT_TEMPL_VAR",
    "FORMAT_TEMPL_WHITESPACE",
    "FORMAT_TEMPL_TOKEN",
    "FORMAT_TEMPL_CONTINUED",
    "FORMAT_TEMPL_TEMPL_END",
    "FORMAT_TEMPL_VAR_END",
    "_FORMAT_TEMPL_VAR_ANNOTE_START",
    "FORMAT_TEMPL_PATH_SEP",
    "FORMAT_TEMPL_PROP_SEP",
    "FORMAT_TEMPL_ATT_SEP",
    "_FORMAT_TEMPL_VAR_ANNOTE_END",
    "_FORMAT_TEMPL_LOGIC_START",
    "FORMAT_TEMPL_WITH",
    "FORMAT_TEMPL_FOR",
    "FORMAT_TEMPL_IF",
    "FORMAT_TEMPL_IFNOT",
    "_FORMAT_TEMPL_LOGIC_END",
    "_TYPE_LANG_END",
    NULL
};

status LangTypeStrings_Init(MemCh *m){
    return Lookup_Add(m, TypeStringRanges, _TYPE_LANG_START, (void *)langTypeStrings);
}
