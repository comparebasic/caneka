#include <external.h>
#include <caneka.h>

status BinPin_Tests(MemCh *gm){
    DebugStack_Push(NULL, 0);
    MemCh *m = MemCh_Make();
    status r = READY;

    /* example 
    <a href="http://example.com>click here</a>
    */

    byte bp[] = {
        (BPTYPE_NODE|BINPIN_HASCHILDREN|BINPIN_HASATTR), (BPTYPE_STRING), (BPTYPE_NUMBER),'0','1','a',
            (BPTYPE_STRING|BINPIN_HASCHILDREN), (BPTYPE_NUMBER),'4','h','r','e','f',
            (BPTYPE_STRING|BINPIN_HASCHILDREN|BINPIN_LAST), (BPTYPE_NUMBER|BINPIN_CONTINUED),'1','2','h','t','t','p',':','/','/','e','x','a','m','p','l','e','.','c','o','m',
            (BPTYPE_STRING|BINPIN_LAST), (BPTYPE_NUMBER),'0','A', 'c','l','i','c','k',' ','h','e','r','e',
        0,
    };

    Str *s = Str_CstrRef(m, (char *)bp);

    Abstract *args[] = {
        (Abstract *)s,
        NULL
    };
    Out("BP: &\n", args);

    MemCh_Free(m);
    DebugStack_Pop();

    r |= SUCCESS;

    return r;
}
