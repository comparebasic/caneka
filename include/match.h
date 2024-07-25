enum pat_flags {
    PAT_END = 0,
    PAT_TERM = 1 << 0,
    PAT_SINGLE = 1 << 1,
    PAT_MANY = 1 << 2,
    PAT_ANY = 1 << 3,
    PAT_INVERT = 1 << 4,
    PAT_COUNT = 1 << 5,
    PAT_SET_NOOP = 1 << 6,
    PAT_IGNORE = 1 << 7,
    PAT_WILDCOUNT = 1 << 8,
    PAT_ALL = 1 << 9,
};

enum match_flags {
    ANCHOR_UNTIL = 1 << 0,
    ANCHOR_CONTAINS = 1 << 1,
    ANCHOR_START = 1 << 2,
};

#define patText \
    PAT_ANY, '\t', '\t', PAT_ANY, '\r', '\r', PAT_ANY, '\n', '\n', PAT_INVERT, 0, 31, \
    PAT_MANY|PAT_ALL|PAT_TERM, 0, 0, PAT_END, 0, 0

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
    int length;
    int position;
    int count;
    int remaining;
    int jump;
} Match;

Match *Match_Make(MemCtx *m, String *s, word flags);
Match *Match_Pattern(MemCtx *m, byte *defs);
status Match_Feed(Match *mt, byte c);
status Match_FeedEnd(Match *mt);
