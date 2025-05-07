#include <external.h>
#include <caneka.h>

Node *Node_Make(MemCh *m, word flags, Node *parent){
    Node *node = (Node *)MemCh_Alloc(m, sizeof(Node));
    node->type.of = TYPE_NODE;
    node->type.state = flags;
    node->parent = parent;
    printf("making node parent: %p\n", parent);
    return node;
}
