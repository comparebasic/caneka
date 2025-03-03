#include <external.h>
#include <caneka.h>

MessSet *MessSet_Make(MemCtx *m){
    MessSet *mset = (MessSet *)MemCtx_Alloc(m, sizeof(MessSet));
    mset->type.of = TYPE_MESSSET;
    mset->root = Mess_Make(m, 0,TYPE_MESS_ELEMENT);
    return mset;
}
