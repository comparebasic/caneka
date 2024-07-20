enum range_flags {
    PAT_END = 0,
    PAT_TERM = 1 << 0,
    PAT_MANY = 1 << 1,
    PAT_ANY = 1 << 2,
    PAT_INVERT = 1 << 3,
    PAT_COUNT = 1 << 4,
    ANCHOR_UNTIL = 1 << 5,
    ANCHOR_CONTAINS = 1 << 6,
    ANCHOR_START = 1 << 7,
};

#define PAT_WHITESPACE(flags) \
    {(flags), ' ', ' '}, \
    {(flags), '\n', '\n'}, \
    {(flags), '\r', '\r'}, \
    {(flags)|PAT_TERM, '\t', '\t'}

#define PAT_INT \
    PAT_TERM, '0', '9'

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
    word defPosition;
} Match;

Match *Match_Make(MemCtx *m, String *s, word flags);
Match *Match_MakePat(MemCtx *m, byte *defs, word npats, word flags);
int Match_PatLength(PatCharDef *def);
status Match_Feed(Match *mt, byte c);
void Match_Reset(Match *mt);
