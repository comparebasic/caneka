#include <external.h>
#include <caneka.h>
#include <test_module.h>

status IoUtilsRelativize_Tests(MemCh *m){
    Debug_Push(m, NULL);
    status r = READY;
    void *args[5];

    Buff *bf = Buff_Make(m, ZERO);

    StrVec *base = IoPath(m, "/base/sub/one/two/three/file.txt");
    StrVec *noop = IoPath(m, "/base/sub/./one/two/./three/file.txt");
    StrVec *back = IoPath(m, "/base/sub/one/two/three/four/../file.txt");
    StrVec *illegal = IoPath(m, "/base/../../one/two/three/file.txt");

    void *ar[] = {
        base,
        noop,
        back,
        illegal,
        NULL
    };

    Out("^p.@\n@\n@\n@\n^0\n", ar);

    IoUtil_Relativise(m, base);
    IoUtil_Relativise(m, noop);
    IoUtil_Relativise(m, back);
    IoUtil_Relativise(m, illegal);

    Return(m, r);
}
