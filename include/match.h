enum pat_flags {
    PAT_END = 0, /* E */
    PAT_TERM = 1 << 0, /* T */
    PAT_OPTIONAL = 1 << 1, /* S */
    PAT_MANY = 1 << 2, /* M */
    PAT_ANY = 1 << 3, /* N */
    PAT_INVERT = 1 << 4, /* I */
    PAT_COUNT = 1 << 5, /* C */
    PAT_SET_NOOP = 1 << 6, /* N */
    PAT_IGNORE = 1 << 7, /* G */
    PAT_WILDCOUNT = 1 << 8, /* W */
    PAT_ALL = 1 << 9, /* A */
};

enum match_flags {
    ANCHOR_UNTIL = 1 << 0,
    ANCHOR_CONTAINS = 1 << 1,
    ANCHOR_START = 1 << 2,
};

#define patText \
    PAT_OPTIONAL, '\t', '\t', PAT_OPTIONAL, '\r', '\r', PAT_OPTIONAL, '\n', '\n', PAT_INVERT|PAT_MANY|PAT_TERM, 0, 31

typedef struct range_chardef {
    word flags;
    word from;
    word to;
} PatCharDef;

typedef struct match {
    Type type; 
    word flags;
    union {
        PatCharDef *pat;
        String *s;
    } def;
    i8 position;
    i8 length;
    i8 jump;
    i16 remaining;
    int count;
} Match;

Match *Match_Make(MemCtx *m, String *s, word flags);
status Match_SetPattern(Match *mt, PatCharDef def[]);
status Match_SetString(Match *mt, String *s);
status Match_Feed(Match *mt, byte c);
status Match_FeedEnd(Match *mt);
