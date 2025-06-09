#define patText \
    {PAT_OPTIONAL|PAT_MANY, '\t', '\t'}, {PAT_OPTIONAL|PAT_MANY, '\r', '\r'}, {PAT_OPTIONAL|PAT_MANY, '\n', '\n'}, {PAT_INVERT|PAT_MANY|PAT_TERM, 0, 31}

#define patAnyText \
    {PAT_OPTIONAL|PAT_ANY, '\t', '\t'}, {PAT_OPTIONAL|PAT_ANY, '\r', '\r'}, {PAT_OPTIONAL|PAT_ANY, '\n', '\n'}, {PAT_INVERT|PAT_ANY|PAT_TERM, 0, 31}

#define patWhiteSpace \
    PAT_MANY, '\t', '\t', PAT_MANY, '\r', '\r', PAT_MANY, '\n', '\n', PAT_MANY|PAT_TERM, ' ', ' ' 

#define TEXT_DEF patText, PAT_END, 0, 0
#define NL_DEF PAT_TERM, '\n', '\n', PAT_END, 0, 0
#define WS_REQUIRED PAT_MANY, ' ',' ', PAT_MANY, '\t','\t', \
    PAT_MANY, '\r','\r', PAT_MANY|PAT_TERM, '\n','\n'
#define WS_OPTIONAL PAT_OPTIONAL|PAT_ANY, ' ',' ', PAT_OPTIONAL|PAT_ANY, '\t','\t', \
    PAT_OPTIONAL|PAT_ANY, '\r','\r', PAT_OPTIONAL|PAT_ANY|PAT_TERM, '\n','\n'

#define WS_INVERT_MANY PAT_INVERT|PAT_MANY, ' ',' ', PAT_INVERT|PAT_MANY, '\t','\t', \
    PAT_INVERT|PAT_MANY,'\r','\r', PAT_INVERT|PAT_MANY, '\n','\n'

#define UPPER_DEF PAT_TERM, 'A', 'Z' 

#define patTextDef \
    {PAT_OPTIONAL|PAT_MANY, '\t', '\t'}, {PAT_OPTIONAL|PAT_MANY, '\r', '\r'}, {PAT_OPTIONAL|PAT_MANY, '\n', '\n'}, {PAT_INVERT|PAT_MANY|PAT_TERM, 0, 31}

#define patTextAny \
    PAT_OPTIONAL|PAT_ANY, '\t', '\t', PAT_OPTIONAL|PAT_ANY, '\r', '\r', PAT_OPTIONAL|PAT_ANY, '\n', '\n', PAT_INVERT|PAT_ANY|PAT_TERM, 0, 31


#define patWhiteSpace \
    PAT_MANY, '\t', '\t', PAT_MANY, '\r', '\r', PAT_MANY, '\n', '\n', PAT_MANY|PAT_TERM, ' ', ' ' 

#define TEXT_DEF patText, PAT_END, 0, 0
#define NL_DEF PAT_TERM, '\n', '\n', PAT_END, 0, 0
#define WS_REQUIRED PAT_MANY, ' ',' ', PAT_MANY, '\t','\t', \
    PAT_MANY, '\r','\r', PAT_MANY|PAT_TERM, '\n','\n'
#define WS_OPTIONAL PAT_OPTIONAL|PAT_ANY, ' ',' ', PAT_OPTIONAL|PAT_ANY, '\t','\t', \
    PAT_OPTIONAL|PAT_ANY, '\r','\r', PAT_OPTIONAL|PAT_ANY|PAT_TERM, '\n','\n'

#define WS_INVERT_MANY PAT_INVERT|PAT_MANY, ' ',' ', PAT_INVERT|PAT_MANY, '\t','\t', \
    PAT_INVERT|PAT_MANY,'\r','\r', PAT_INVERT|PAT_MANY, '\n','\n'

#define UPPER_DEF PAT_TERM, 'A', 'Z' 

