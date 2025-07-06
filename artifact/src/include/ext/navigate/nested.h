typedef struct nested {
    Type type;
    Iter &it;
}

Nested *Nested_Make(m, Abstract *root);
status Nested_Indent(Nested *nd);
status Nested_Outdent(Nested *nd);
status Nested_If(Nested *nd);
status Nested_IfNot(Nested *nd);
status Nested_SetFor(Nested *nd);
Abstract *Nested_Next(Nested *nd);
