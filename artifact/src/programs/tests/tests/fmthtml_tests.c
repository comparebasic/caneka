#include <external.h>
#include <caneka.h>

static char *expectedCstr = ""
"<H1>Hidy Ho</H1>\n"
"<P>And this is a first paragraph with a second line.</P>\n"
"<P>Second single sentance with details and a link <A href=\"http://example.com\">here</A>.</P>\n"
"<IMG alt=\"Image one\" src=\"image.png\" /><UL>\n"
"  <LI>bullet one.</LI>\n"
"  <LI>bullet two with two lines.</LI>\n"
"  <LI>bullet three with two lines as well.</LI>\n"
"  <LI>bullet four.</LI>\n"
"</UL>\n"
"<H2>Sub Section</H2>\n"
"<P>A paragraph in the sub section.</P>\n"
"<H1>Table Title</H1>\n"
"<TABLE>\n"
"  <THEAD><TR><TD>Column A</TD><TD>Column B</TD><TD>Column C</TD></TR></THEAD>\n"
"  <TR><TD>Apple</TD><TD>1</TD><TD>37</TD></TR>\n"
"  <TR><TD>Bannana</TD><TD>2</TD><TD>39</TD></TR>\n"
"  <TR><TD>Cantelope</TD><TD>3</TD><TD>39</TD></TR>\n"
"</TABLE>\n"
"<P>And the final text here.</P>\n"
;

status FmtHtml_Tests(MemCh *gm){
    DebugStack_Push(NULL, 0);
    status r = READY;
    MemCh *m = MemCh_Make();
    Str *s = NULL; 

    Str *path = IoUtil_GetAbsPath(m, Str_CstrRef(m, "./examples/example.fmt"));
    path->type.state |= DEBUG;
    StrVec *content = File_ToVec(m, path);

    Cursor *curs = Cursor_Make(m, content); 

    Roebling *rbl = NULL;
    rbl = FormatFmt_Make(m, curs, NULL);
    Roebling_Run(rbl);

    Buff *bf = Buff_Make(m, BUFF_STRVEC); 
    
    Fmt_ToHtml(bf, rbl->mess);
    r |= Test(rbl->mess->transp->type.state & SUCCESS, 
        "Fmt to HTML has status SUCCESS", NULL);

    s = Str_CstrRef(m, expectedCstr);
    s->type.state |= DEBUG;
    boolean matches = Equals((Abstract *)s, (Abstract *)bf->v);
    if(!matches){
        Abstract *args[] = {
            (Abstract *)s,
            (Abstract *)bf->v,
            NULL
        };
        Out("^r.Mismatch:\n  Expected:&\n  Actual: ^E.&^0.", args);
    }
    r |= Test(matches, 
        "Fmt to HTML has expected html content", NULL);

    MemCh_Free(m);
    DebugStack_Pop();
    return r;
}
