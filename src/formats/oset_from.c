#include <external.h>
#include <caneka.h>

Abstract *String_FromOset(MemCtx *m, FmtDef *odef, FmtCtx *o, String *key, Abstract *a){
    String *s = (String *)asIfc(a, TYPE_STRING);
    return (Abstract *)s;
}

Abstract *Blank_FromOset(MemCtx *m, FmtDef *odef, FmtCtx *o, String *key, Abstract *a){
    String *s = (String *)asIfc(a, TYPE_STRING);
    Blank *b = Blank_Make(m);
    b->length = s->length;
    return (Abstract *)b;
}

Abstract *I64_FromOset(MemCtx *m, FmtDef *odef, FmtCtx *o, String *key, Abstract *a){
    String *s = (String *)asIfc(a, TYPE_STRING);
    return (Abstract *)I64_Wrapped(m, I64_FromString((String *)a));
}

Abstract *Iter_FromOset(MemCtx *m, FmtDef *odef, FmtCtx *o, String *key, Abstract *a){
    return (Abstract *)o->item->value;
}

Abstract *Table_FromOset(MemCtx *m, FmtDef *odef, FmtCtx *o, String *key, Abstract *a){
    return (Abstract *)o->item->value;
}

Abstract *Span_FromOset(MemCtx *m, FmtDef *odef, FmtCtx *o, String *key, Abstract *a){
    return (Abstract *)o->item->value;
}

Abstract *Auth_FromOset(MemCtx *m, FmtDef *odef, FmtCtx *o, String *key, Abstract *a){
    Span *tbl = as(o->item->value, TYPE_TABLE);
    Auth *auth = Auth_Make(m, NULL, NULL, NULL);
    Span *salt = (Span *)Table_Get(tbl, (Abstract *)String_Make(m, bytes("salt")));
    if(salt != NULL){
        auth->saltenc = as(salt, TYPE_ENC_PAIR);
    }
    String *digest = (String *)Table_Get(tbl, (Abstract *)String_Make(m, bytes("digest")));
    if(digest != NULL){
        auth->digest = String_FromHex(m, digest);
    }
    
    return (Abstract *)auth;
}

Abstract *EncPair_FromOset(MemCtx *m, FmtDef *odef, FmtCtx *o, String *key, Abstract *a){
    Span *tbl = as(o->item->value, TYPE_TABLE);
    String *enc = NULL;
    String *enchex = (String *)Table_Get(tbl, (Abstract *)String_Make(m, bytes("enc")));
    if(enchex != NULL){
        enc = String_FromHex(m, enchex);
        Debug_Print((void *)enc, 0, "Enc resurrected: ", 0, TRUE);
        printf("\n");
    }
    Single *l_sg = (Single *)Table_Get(tbl, (Abstract *)String_Make(m, bytes("length")));
    int l = 0;
    if(l_sg != NULL){
        l = (int)l_sg->val.value;
    }
    return (Abstract *)EncPair_Make(m,
        (String *)Table_Get(tbl, (Abstract *)String_Make(m, bytes("key"))),
        enc,
        NULL,
        NULL,
        l
    );
}

Abstract *FilePath_FromOset(MemCtx *m, FmtDef *odef, FmtCtx *o, String *key, Abstract *a){
    if(a->type.of == TYPE_FILE){
        return a;
    }
    String *s = (String *)asIfc(a, TYPE_STRING);

    File *file = MemCtx_Alloc(m, sizeof(File));
    File_Init(file, s, NULL, NULL);

    file->type.state = s->type.state;

    return (Abstract *)file;
}
