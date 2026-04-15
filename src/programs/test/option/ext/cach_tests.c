#include <external.h>
#include <caneka.h>
#include <test_module.h>

char *oneCstr = "Hi there ${name}"
    ;

char *twoCstr = "Hi there ${name}${show}, The temperature is "
    "${details.temp}${details.unit}"
    "${show}"
    "."
    ;

status Cash_Tests(MemCh *m){
    status r = READY;

    Cursor *one = Cursor_Make(m, Sv(m, oneCstr));
    Cursor *two = Cursor_Make(m, Sv(m, twoCstr));

    void *ar[] = {
        one, two, NULL
    };
    Out("^p.One @\nTwo @\n", ar);

    Roebling *parser = CashParser_Make(m, one, ZERO);
    parser->type.state |= DEBUG;
    Roebling_Run(parser);

    return r;
}
