#include <external.h>
#include <caneka.h>

Node *Node_Make(MemCh *m, word flags){
    Node *node = (Node *)MemCh_Alloc(m, sizeof(Node));
    node->type.of = TYPE_NODE;
    node->type.state = flags;
    return node;
}
