#define PAT_CHAR_MAX_LENGTH 64

enum match_flags {
    MATCH_GOTO = 1 << 8,
    /*MATCH__ = 1 << 9,*/
    MATCH_TERM_FOUND = 1 << 10,
    MATCH_KO_INVERT = 1 << 11,
    MATCH_KO = 1 << 12,
    MATCH_SEARCH = 1 << 13, 
    MATCH_LEAVE = 1 << 14,
    MATCH_ACCEPT_EMPTY = 1 << 15
};

typedef struct match {
    Type type; 
    i32 remaining;
    i32 jump;
    word counter;
    word captureKey;
    struct {
        PatCharDef *startDef;
        PatCharDef *endDef;
        PatCharDef *curDef;
        PatCharDef *startTermDef;
    } pat;
    Snip snip;
    Span *backlog;
} Match;

status Match_AddBoundrySnip(MemCh *m, Match *mt);
Match *Match_Make(MemCh *m, PatCharDef *def, Span *backlog);
status Match_Feed(MemCh *m, Match *mt, byte c);
status Match_FeedStrVec(MemCh *m, Match *mt, StrVec *v, i32 offset);
status Match_FeedEnd(MemCh *m, Match *mt);
status Match_SetCount(Match *mt, i32 count);
