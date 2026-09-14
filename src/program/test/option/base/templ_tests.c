#include <external.h>
#include <caneka.h>
#include <test_module.h>

status Templ_Tests(MemCh *m){
    void *args[2];
    status r = READY;
    m->level++;

    Templ *templ = Templ_FromCstr(m, "^p.Hi There &, it's % degrees @{location}^0\n");
    void *ar[] = {
        templ,
        NULL
    };
    Out("^p.@^0\n", ar);

    r |= ERROR;

    return r;
}
