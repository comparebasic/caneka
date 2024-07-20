Single *Mark_Make(MemCtx *m, word mark){
    Single *mark = MemCtx_Alloc(rbl->m, sizeof(Single));
    mark->type.of = TYPE_RBL_MARK;
    mark->value = mark;
    return mark;
}
