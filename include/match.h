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

#define PAT_WHITESPACE(flags) \
    {(flags), ' ', ' '}, \
    {(flags), '\n', '\n'}, \
    {(flags), '\r', '\r'}, \
    {(flags)|PAT_TERM, '\t', '\t'}

#define PAT_INT \
    PAT_TERM, '0', '9'

#define patText \
    PAT_ANY, '\t', '\t', PAT_ANY, '\r', '\r', PAT_ANY, '\n', '\n', PAT_INVERT, 0, 31, \
    PAT_MANY|PAT_ALL|PAT_TERM, 0, 0, PAT_END, 0, 0

#define patTextLength 5

#define PAT_FLOAT \
    PAT_TERM, '0', '9', \
    PAT_ANY|PAT_TERM, '0', '9', \
    PAT_TERM, '.', '.', \
    PAT_MANY|PAT_TERM, '0', '9'

#define PAT_ALPHA \
    PAT_ANY, 'a', 'z' \
    PAT_ANY|PAT_TERM, 'A', 'Z'

#define PAT_UPPER_ALPHA \
    PAT_ANY|PAT_TERM, 'A', 'Z'

#define PAT_LOWER_ALPHA \
    PAT_ANY|PAT_TERM, 'a', 'z'

typedef struct range_chardef {
    word flags;
    word from;
    word to;
} PatCharDef;

typedef struct match {
    Type type; 
    word flags;
    status state;
    String *s; 
    int position;
    int count;
    int remaining;
    word defPosition;
} Match;

Match *Match_Make(MemCtx *m, String *s, word flags);
Match *Match_MakePat(MemCtx *m, byte *defs, word npats, word flags);
int Match_PatLength(PatCharDef *def);
status Match_Feed(Match *mt, byte c);
void Match_Reset(Match *mt);
status Match_FeedEnd(Match *mt);
