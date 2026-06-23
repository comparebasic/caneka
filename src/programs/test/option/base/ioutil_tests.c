#include <external.h>
#include <caneka.h>
#include <test_module.h>

status IoUtilsRelativize_Tests(MemCh *m){
    Debug_Push(m, NULL);
    status r = READY;
    void *args[7];

    Buff *bf = Buff_Make(m, ZERO);

    StrVec *base = IoPath(m, "/base/sub/one/two/three/file.txt");
    StrVec *noop = IoPath(m, "/base/sub/./one/two/./three/file.txt");
    StrVec *back = IoPath(m, "/base/sub/one/two/three/four/../file.txt");
    StrVec *illegal = IoPath(m, "/base/../../sub/one/two/three/file.txt");
    
    StrVec *vecs[] = {
        base,
        noop,
        back,
        NULL
    };

    IoUtil_Relativise(m, base);
    IoUtil_Relativise(m, noop);
    IoUtil_Relativise(m, back);

    Str *expected = S(m, "/base/sub/one/two/three/file.txt");
    Single *sg = I32_Wrapped(m, 0);

    StrVec **ptr = vecs;
    while(*ptr != NULL){
        args[0] = sg;
        args[1] = expected;
        args[2] = Ifc(m, *ptr, TYPE_STR);
        args[3] = NULL;
        r |= Test(Equals(args[1], args[2]), 
            "Relativised str $ is expected &, have &", args);
        sg->val.i++;
        ptr++;
    }

    StrVec *local = IoPath(m, "./base/sub/one/two/three/file.txt");
    IoUtil_Relativise(m, local);

    StrVec *illegalOrig = Clone(m, illegal);
    IoUtil_Relativise(m, illegal);

    args[0] = Type_StateVec(m, illegal->type.of, illegal->type.state);
    args[1] = illegalOrig;
    args[2] = NULL;

    r |= Test(illegal->type.state & ERROR, 
        "Relativised path above root is flagged with ERROR, have @ for @", args);

    args[0] = S(m, "base/sub/one/two/three/file.txt");
    args[1] = Ifc(m, local, TYPE_STR);
    args[2] = NULL;

    r |= Test(Equals(args[0], args[1]), 
        "Relativised local string is expected &, have &", args);

    args[0] = Type_StateVec(m, local->type.of, local->type.state);
    args[1] = NULL;
    r |= Test(local->type.state & NOOP, 
        "Relativised local string has NOOP flag, have @", args);

    Return(m, r);
}
