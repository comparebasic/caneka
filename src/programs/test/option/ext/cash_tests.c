#include <external.h>
#include <caneka.h>
#include <test_module.h>

char *oneCstr = "Hi there ${name}!"
    ;

char *twoCstr = "Hi there, ${name}.${details.temp?} The temperature is "
    "${details.temp}${details.unit}.${/}"
    ;

status Cash_Tests(MemCh *m){
    status r = READY;

    void *args[3];

    Cursor *one = Cursor_Make(m, Sv(m, oneCstr));
    Cursor *two = Cursor_Make(m, Sv(m, twoCstr));

    Span *cash = Cash_Prepare(m, one);
    Table *tbl = Table_Make(m);
    Table_Set(tbl, K(m, "name"), S(m, "Gerry"));

    Buff *bf = Buff_Make(m, ZERO);
    Cash_Out(cash, bf, tbl);

    args[0] = bf->v;
    args[1] = NULL;
    Out("^y.First @\n", args);

    cash = Cash_Prepare(m, two);

    tbl = Table_Make(m);
    Table_Set(tbl, K(m, "name"), S(m, "Gerry"));

    bf = Buff_Make(m, ZERO);
    Cash_Out(cash, bf, tbl);

    args[0] = cash;
    args[1] = NULL;
    Out("^p.Second &\n", args);

    args[0] = bf->v;
    args[1] = NULL;
    Out("^y.Second @\n", args);

    Table *details = Table_Make(m);
    Table_Set(details, K(m, "temp"), I32_Wrapped(m, 31));
    Table_Set(details, K(m, "unit"), S(m, "F"));
    Table_Set(tbl, K(m, "details"), details);

    bf = Buff_Make(m, ZERO);
    Cash_Out(cash, bf, tbl);

    args[0] = bf->v;
    args[1] = NULL;
    Out("^y.Third @\n", args);


    return r;
}
