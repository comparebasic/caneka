
Abstract *NavItem_Make(MemCh *m, TranspFile *tp){
    Hashed *h = Hashed_Make(m, (Abstract *)tp->name);
    h->value = Ptr_Wrapped(m, tp, TYPE_HTTP_NAVITEM);
    return h;
}

status NavItem_ClsInit(MemCh *m){
    ClassDef *cls = ClassDef_Make(m);
    /* add methods */ 
    return NOOP;
}
