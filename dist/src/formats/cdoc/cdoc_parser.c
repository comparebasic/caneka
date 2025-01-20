PatCharDef commentDef[] = {
    {PAT_TERM, '/', '/'},
    {PAT_TERM, '*', '*'},
    patText,
    {PAT_TERM, '*', '*'},
    {PAT_TERM, '/', '/'},
    {PAT_END, 0, 0},
};

PatCharDef funcSigDef[] = {
    {PAT_MANY, 'A', 'Z'}, {PAT_MANY, 'a', 'z'}, {PAT_MANY, '0', '9'}, {PAT_MANY, '_', '_'}, {PAT_MANY, ' ', ' '}, {PAT_MANY, '*', '*'}, {PAT_MANY, '\t', '\t'}, {PAT_MANY, '\n', '\n'}, {PAT_MANY|PAT_TERM, '\r', '\r'},
    {PAT_TERM, '(', '('},
    {PAT_MANY, 'A', 'Z'}, {PAT_MANY, 'a', 'z'}, {PAT_MANY, '0', '9'}, {PAT_MANY, '*', '*'}, {PAT_MANY, '_', '_'}, {PAT_MANY, ' ', ' '}, {PAT_MANY, '\t', '\t'}, {PAT_MANY, '\n', '\n'}, {PAT_MANY|PAT_TERM, '\r', '\r'},
    {PAT_TERM, ')', ')'},
    {PAT_TERM, ';', ';'},
    {PAT_END, 0, 0},
};

PatCharDef typeDef[] = {
    {PAT_TERM, 't', 't'},
    {PAT_TERM, 'y', 'y'},
    {PAT_TERM, 'p', 'p'},
    {PAT_TERM, 'e', 'e'},
    {PAT_TERM, 'd', 'd'},
    {PAT_TERM, 'e', 'e'},
    {PAT_TERM, 'f', 'f'},
    {PAT_TERM|PAT_MANY, ' ', ' '},
    {PAT_TERM, 's', 's'},
    {PAT_TERM, 't', 't'},
    {PAT_TERM, 'r', 'r'},
    {PAT_TERM, 'u', 'u'},
    {PAT_TERM, 'c', 'c'},
    {PAT_TERM, 't', 't'},
    {PAT_MANY, 'A', 'Z'}, {PAT_MANY, 'a', 'z'}, {PAT_MANY, '0', '9'}, {PAT_MANY, '*', '*'}, {PAT_MANY, '_', '_'}, {PAT_MANY, ' ', ' '}, {PAT_MANY, '\t', '\t'}, {PAT_MANY, '\n', '\n'}, {PAT_MANY|PAT_TERM, '\r', '\r'},
    {PAT_TERM, '{', '{'},
    {PAT_MANY, 'A', 'Z'}, {PAT_MANY, 'a', 'z'}, {PAT_MANY, '0', '9'}, {PAT_MANY, '*', '*'}, {PAT_MANY, '_', '_'}, {PAT_MANY, ' ', ' '}, {PAT_MANY, '\t', '\t'}, {PAT_MANY, '\n', '\n'}, {PAT_MANY|PAT_TERM, '\r', '\r'},
    {PAT_TERM, '}', '}'},
    {PAT_TERM|PAT_MANY, ' ', ' '}
    {PAT_MANY, 'A', 'Z'}, {PAT_MANY, 'a', 'z'},
    {PAT_TERM|PAT_MANY, ' ', ' '}
    {PAT_TERM, ';', ';'},
    {PAT_END, 0, 0},
};

PatCharDef wsDef[] = {
    {PAT_ANY, ' ', ' '},
    {PAT_ANY, '\t', '\t'},
    {PAT_ANY, '\r', '\r'},
    {PAT_ANY, '\n', '\n'},
    {PAT_END, 0, 0},
};
