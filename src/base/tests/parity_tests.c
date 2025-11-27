#include <external.h>
#include <caneka.h>

status Parity_Tests(MemCh *m){
    DebugStack_Push(NULL, 0);
    status r = READY;
    void *args[4];

    quad hparity = 0;

    StrVec *a = StrVec_Make(m);
    StrVec_Add(a, S(m, "Once u"));
    StrVec_Add(a, S(m, "pon a time. There was a handsome pot of coffee."));
    StrVec_Add(a, S(m, " Who met a voluptous creamer. Who had a spicy"));
    StrVec_Add(a, S(m, " sidekick."));
    hparity = HalfParity_FromVec(a);
    args[0] = U32_Wrapped(m, hparity);
    args[1] = NULL;
    r |= Test(hparity != 0, "HalfParity is non zero, have $", args);
    args[0] = a;
    args[1] = NULL;
    r |= Test(HalfParity_Compare(hparity, a), "HalfParity matches for, @", args);

    StrVec *b = StrVec_Make(m);
    StrVec_Add(b, S(m, "O"));
    StrVec_Add(b, S(m, "nce u"));
    StrVec_Add(b, S(m, "pon a time. There was a handsome pot of coffee."));
    StrVec_Add(b, S(m, " Who met a voluptous creamer. Who had a spicy"));
    StrVec_Add(b, S(m, " sidekick."));

    quad hnew = HalfParity_FromVec(b);
    args[0] = b;
    args[1] = U32_Wrapped(m, hparity);
    args[2] = U32_Wrapped(m, hnew);
    args[3] = NULL;
    r |= Test(hnew == hparity, "HalfParity matches for, @, $ vs $", args);

    StrVec *c = StrVec_Make(m);
    StrVec_Add(c, S(m, "Once upon a time. There was a handsome pot of coffee."
        " Who met a voluptous creamer. Who had a spicy sidekick."));

    hnew = HalfParity_FromVec(c);
    args[0] = c;
    args[1] = U32_Wrapped(m, hparity);
    args[2] = U32_Wrapped(m, hnew);
    args[3] = NULL;
    r |= Test(hnew == hparity, "HalfParity matches for, @, $ vs $", args);

    Str *d = S(m, "Once upon a time. There was a handsome pot of coffee."
        " Who met a voluptous creamer. Who had a spicy sidekick.");

    hnew = HalfParity_From(d);
    args[0] = c;
    args[1] = U32_Wrapped(m, hparity);
    args[2] = U32_Wrapped(m, hnew);
    args[3] = NULL;
    r |= Test(hnew == hparity, "Str HalfParity matches for, @, $ vs $", args);

    util parity = 0;

    a = StrVec_Make(m);
    StrVec_Add(a, S(m, "Once u"));
    StrVec_Add(a, S(m, "pon a time. There was a handsome pot of coffee."));
    StrVec_Add(a, S(m, " Who met a voluptous creamer. Who had a spicy"));
    StrVec_Add(a, S(m, " sidekick."));
    parity = Parity_FromVec(a);
    args[0] = U32_Wrapped(m, parity);
    args[1] = NULL;
    r |= Test(parity != 0, "Parity is non zero, have $", args);
    args[0] = a;
    args[1] = NULL;
    r |= Test(Parity_Compare(parity, a), "Parity matches for, @", args);

    b = StrVec_Make(m);
    StrVec_Add(b, S(m, "O"));
    StrVec_Add(b, S(m, "nce u"));
    StrVec_Add(b, S(m, "pon a time. There was a handsome pot of coffee."));
    StrVec_Add(b, S(m, " Who met a voluptous creamer. Who had a spicy"));
    StrVec_Add(b, S(m, " sidekick."));

    util new = Parity_FromVec(b);
    args[0] = b;
    args[1] = U32_Wrapped(m, parity);
    args[2] = U32_Wrapped(m, new);
    args[3] = NULL;
    r |= Test(new == parity, "Parity matches for, @, $ vs $", args);

    c = StrVec_Make(m);
    StrVec_Add(c, S(m, "Once upon a time. There was a handsome pot of coffee."
        " Who met a voluptous creamer. Who had a spicy sidekick."));

    new = Parity_FromVec(c);
    args[0] = c;
    args[1] = U32_Wrapped(m, parity);
    args[2] = U32_Wrapped(m, new);
    args[3] = NULL;
    r |= Test(new == parity, "Parity matches for, @, $ vs $", args);

    d = S(m, "Once upon a time. There was a handsome pot of coffee."
        " Who met a voluptous creamer. Who had a spicy sidekick.");

    new = Parity_From(d);
    args[0] = c;
    args[1] = U32_Wrapped(m, parity);
    args[2] = U32_Wrapped(m, new);
    args[3] = NULL;
    r |= Test(new == parity, "Str Parity matches for, @, $ vs $", args);

    DebugStack_Pop();
    return r;
}
