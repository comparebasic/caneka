enum pat_flags {
    PAT_END = 0, /* E */
    PAT_TERM = 1 << 0, /* X */
    PAT_OPTIONAL = 1 << 1, /* P */
    PAT_MANY = 1 << 2, /* M */
    PAT_ANY = 1 << 3, /* N */
    PAT_INVERT = 1 << 4, /* I */
    PAT_COUNT = 1 << 5, /* C */
    PAT_SET_NOOP = 1 << 6, /* U */
    PAT_IGNORE = 1 << 7, /* G */
    PAT_WILDCOUNT = 1 << 8, /* W */
    PAT_ALL = 1 << 9, /* A */
    PAT_NORMAL = 1 << 10, /* */
    PAT_OR = 1 << 11, /* */
    PAT_KO = 1 << 12, /* */
};

enum match_flags {
    ANCHOR_UNTIL = 1 << 0,
    ANCHOR_CONTAINS = 1 << 1,
    ANCHOR_START = 1 << 2,
};

#define patText \
    PAT_OPTIONAL, '\t', '\t', PAT_OPTIONAL, '\r', '\r', PAT_OPTIONAL, '\n', '\n', PAT_INVERT|PAT_MANY|PAT_TERM, 0, 31

#define TEXT_DEF patText, PAT_END, 0, 0
#define NL_DEF PAT_TERM, '\n', '\n', PAT_END, 0, 0
#define WS_REQUIRED PAT_MANY, ' ',' ', PAT_MANY, '\t','\t', \
    PAT_MANY, '\r','\r', PAT_MANY|PAT_TERM, '\n','\n'
#define WS_OPTIONAL PAT_OPTIONAL|PAT_ANY, ' ',' ', PAT_OPTIONAL|PAT_ANY, '\t','\t', \
    PAT_OPTIONAL|PAT_ANY, '\r','\r', PAT_OPTIONAL|PAT_ANY|PAT_TERM, '\n','\n'

#define WS_INVERT_MANY PAT_INVERT|PAT_MANY, ' ',' ', PAT_INVERT|PAT_MANY, '\t','\t', \
    PAT_INVERT|PAT_MANY,'\r','\r', PAT_INVERT|PAT_MANY, '\n','\n'

#define UPPER_DEF PAT_TERM, 'A', 'Z' 

typedef struct range_chardef {
    word flags;
    word from;
    word to;
} PatCharDef;

typedef struct match {
    Type type; 
    union {
        PatCharDef *pat;
        String *s;
    } def;
    i8 position;
    i8 length;
    i8 jump;
    i16 remaining;
    int count;
    int lead;
    status (*dispatch)(struct roebling *rbl);
} Match;

Match *Match_Make(MemCtx *m, String *s, word flags);
status Match_SetPattern(Match *mt, PatCharDef *def);
status Match_SetString(Match *mt, String *s);
status Match_Feed(Match *mt, byte c);
status Match_FeedEnd(Match *mt);
PatCharDef *Match_GetDef(Match *mt);
