#include <external.h>
#include <caneka.h>
#include <test_module.h>

char *oneCstr = "Hi there ${name}!"
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

    Span *cash = Cash_Prepare(m, one);


    Table *tbl = Table_Make(m);
    Table_Set(tbl, K(m, "name"), S(m, "Gerry"));

    Buff *bf = Buff_Make(m, ZERO);
    Cash_Out(cash, bf, tbl);

    void *ar[] = {
       cash, bf->v,
    };
    Out("^p.First @ -> @\n", ar);

    return r;
}
