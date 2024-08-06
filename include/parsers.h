enum parser_cmds {
    BREAK = -2,
};

typedef status (*ParserSetter)(struct structexp *sexp); /* pmk */

typedef status (*ParseFunc)(struct parser *parser, Range *range, void *source);

typedef struct parser {
    Type type;
    word flags;
    int idx;
    int jump;
    int failJump;
    ParseFunc complete;
} Parser; /*rp*/

status Parser_Run(Parser *prs, struct structexp *rbl);
