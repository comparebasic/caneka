#include <external.h>
#include <base_module.h>

static void Templ_render(Buff *bf, Templ *templ, TypedArr *args, void *_a){
    MemCh *m = bf->m;
    Abstract *a = (Abstract *)_a;

    if(a->type.of == TYPE_STR){
        Buff_Add(bf, (Str *)a);
    }else if(a->type.of == TYPE_STRVEC){
        Buff_AddVec(bf, (StrVec *)a);
    }else if(a->type.of == TYPE_HASHED){
        Hashed *h = (Hashed *)a;
        Single *sg = (Single *)h->value;

        if(templ->currentIdx > args->rangeType.range){
            Error(m, FUNCNAME, FILENAME, LINENUMBER,
                "Array for Templ has fewer items than requested index", NULL);
            templ->type.state |= ERROR;
            return;
        }

        Abstract *item = TypedArr_Get(m, args, templ->currentIdx);
        if(item != NULL && h->key != NULL && item->type.of == TYPE_TABLE){
            Table *tbl = (Table *)item;
            item = Table_Get(tbl, h->key);
        }

        if(item == NULL){
            void *ar[] = {
                I32_Wrapped(m, templ->currentIdx),
                NULL
            };
            Error(m, FUNCNAME, FILENAME, LINENUMBER, "Item $ is NULL", ar);
            templ->type.state |= ERROR;
            return;
        }

        templ->currentIdx++;

        ToSFunc func = NULL; 
        if(sg->val.ptr != NULL && item->type.of == sg->objType.of){
            func = (ToSFunc)sg->val.ptr; 
        }else{
            func = (ToSFunc)Lookup_Get(ToStreamLookup, item->type.of);
            sg->val.ptr = func;
            sg->objType.of = item->type.of;
        }

        if(func == NULL){
            Error(m, FUNCNAME, FILENAME, LINENUMBER,
                "ToS func not found in lookup", NULL);
            templ->type.state |= ERROR;
            return;
        }

        func(bf, item, item->type.of, ZERO);
    }else if(a->type.of == TYPE_SPAN){
        Iter it;
        Iter_Init(&it, (Span *)a);
        while((Iter_Next(&it) & END) == 0){
            Templ_render(bf, templ, args, Iter_Get(&it));
        }
    }else{
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "Unsupported type for Templ_PrepTotal", NULL);
        templ->type.state |= ERROR;
    }
}

status Templ_Render(Buff *bf, Templ *templ, TypedArr *args){
    templ->currentIdx = 0;
    Templ_render(bf, templ, args, templ->p);
    return templ->type.state;
}

StrVec *Templ_ToVec(MemCh *m, Templ *templ, TypedArr *args){

    Buff bf;
    memset(&bf, 0, sizeof(Buff));
    bf.type.of = TYPE_BUFF;
    bf.m = m;
    Buff_InitVec(m, &bf, StrVec_Make(m));

    templ->currentIdx = 0;
    Templ_render(&bf, templ, args, templ->p);

    return bf.v;
}

Str *Templ_ToStr(MemCh *m, Templ *templ, TypedArr *args){
    return Ifc(m, Templ_ToVec(m, templ, args), TYPE_STR);
}

Templ *Templ_FromCstr(MemCh *m, char *cstr){
    Str *s = S(m, cstr);
    return Templ_From(m, s->bytes, s->length);
}

Templ *Templ_FromVec(MemCh *m, StrVec *v){
    return NULL;
}

Templ *Templ_From(MemCh *m, byte *b, i16 length){
    Templ *templ = (Templ *)MemCh_Alloc(m, sizeof(Templ));
    templ->type.of = TYPE_TEMPL;
    templ->p = Span_Make(m);

    Str *s = NULL;
    status r = READY;

    Iter it;
    Iter_Init(&it, templ->p);

    byte *token = b;
    word tlength = 0;

    i32 idx = 0;

    byte *end = b+length-1;
    while(b <= end){
        byte c = *b;

        if(b == end && (c == '^' || c == '\\' || c == '%' || c == '@' || c == '&')){
            s = Str_Ref(m, b, length, length, ZERO);
            void *ar[] = {s, NULL};
            Error(m, FUNCNAME, FILENAME, LINENUMBER,
                "Open or escape found at end of string", ar);
            return NULL;
        }

        if(r & (PROCESSING|SUCCESS)){
            if(c == '}'){
                Str *s = Str_Ref(m, token, tlength, tlength, ZERO);
                Hashed *h = Hashed_Make(m, s);
                r &= ~(PROCESSING|SUCCESS);
                h->objType.state = r;
                h->value = Func_Wrapped(m, NULL, ZERO);
                h->orderIdx = idx++;
                Iter_Add(&it, h);
                token = b+1;
                tlength = 0;
                b++;
                r = READY;
                continue;
            }else if((r & PROCESSING) && c == '{'){
                s = Str_Ref(m, token, tlength, tlength, ZERO);
                Iter_Add(&it, s);
                token = b+1;
                tlength = 0;
                r |= SUCCESS;
                goto next;
            }else if((r & SUCCESS) == 0){
                Hashed *h = Hashed_Make(m, NULL);
                r &= ~PROCESSING;
                h->objType.state = r;
                h->value = Func_Wrapped(m, NULL, ZERO);
                h->orderIdx = idx++;
                Iter_Add(&it, h);
                r = READY;
            }
        }

        if(c == '\\'){
            if(b == end){
                break;
            }else{
                b++;
            }
        }else if(c == '^'){
            b++;
            s = Str_Make(m, ANSI_ESCAPE_MAX);
            s->type.state |= STRING_BINARY;
            s->length = Ansi_Consume(m, (char **)&b, end - b, s->bytes, s->alloc);
            Iter_Add(&it, s);
            token = b+1;
            goto next;
        }else if(c == '%'){
            r = PROCESSING;
            goto seg;
        }else if(c == '@'){
            r = (PROCESSING|MORE);
            goto seg;
        }else if(c == '&'){
            r = (PROCESSING|DEBUG);
            goto seg;
        }

        tlength++;

        r &= ~PROCESSING;
        goto next;
seg:
        s = Str_Ref(m, token, tlength, tlength, ZERO);
        Iter_Add(&it, s);
        token = b+1;
        tlength = 0;
next:
        b++;
    }

    return templ;
}

