#include <external.h>
#include <caneka.h>

Chain *HashChain;

static util Hash_Bytes(byte *bt, size_t length){
	util h = 5381;
	util i = 0;
	byte c;
	while (i++ < length) {
		c = *bt++;
		h = (h << 5) + h + (h << 9) + h + (h << 31) + h + (h << 49) + h + c;
	}
    return h;
}

static util Hash_Abstract(Abstract *a){
    return 0;
}

static util Hash_MemPair(MemPair *mp){
    return Hash_Bytes((byte *)mp, sizeof(MemPair));
}

static util Hash_Match(Abstract *a){
    return 0;
}

static util Hash_PatMatch(Abstract *a){
    return 0;
}

static util Hash_PatCharDef(Abstract *a){
    return 0;
}

static util Hash_String(Abstract *a){
    String *s = (String *)as(a, TYPE_STRING_CHAIN);
    return Hash_Bytes(s->bytes, s->length);
}

static util Hash_StringFixed(Abstract *a){
    String *s = (String *)as(a, TYPE_STRING_FIXED);
    return Hash_Bytes(s->bytes, s->length);
}

static util Hash_SCursor(Abstract *a){
    return 0;
}

static util Hash_Range(Abstract *a){
    return 0;
}

static util Hash_Req(Abstract *a){
    return 0;
}

static util Hash_Slab(Abstract *a){
    return 0;
}

static util Hash_Span(Abstract *a){
    return 0;
}

static status populateHash(MemCtx *m, Lookup *lk){
    status r = READY;
    r |= Lookup_Add(m, lk, TYPE_ABSTRACT, (void *)Hash_Abstract);
    r |= Lookup_Add(m, lk, TYPE_MATCH, (void *)Hash_Match);
    r |= Lookup_Add(m, lk, TYPE_PATMATCH, (void *)Hash_PatMatch);
    r |= Lookup_Add(m, lk, TYPE_PATCHARDEF, (void *)Hash_PatCharDef);
    r |= Lookup_Add(m, lk, TYPE_PATMATCH, (void *)Hash_Match);
    r |= Lookup_Add(m, lk, TYPE_STRING_CHAIN, (void *)Hash_String);
    r |= Lookup_Add(m, lk, TYPE_STRING_FIXED, (void *)Hash_StringFixed);
    r |= Lookup_Add(m, lk, TYPE_SCURSOR, (void *)Hash_SCursor);
    r |= Lookup_Add(m, lk, TYPE_RANGE, (void *)Hash_Range);
    r |= Lookup_Add(m, lk, TYPE_REQ, (void *)Hash_Req);
    r |= Lookup_Add(m, lk, TYPE_SLAB, (void *)Hash_Slab);
    r |= Lookup_Add(m, lk, TYPE_SPAN, (void *)Hash_Span);
    r |= Lookup_Add(m, lk, TYPE_MEMPAIR, (void *)Hash_MemPair);
    return r;
}

util Get_Hash(Abstract *a){
    if(a->type.of == TYPE_HASHED){
        Hashed *h = (Hashed *)a;
        return h->id;
    }

    HashFunc func = (HashFunc)Chain_Get(HashChain, a->type.of);
    if(func != NULL){
        return func(a);
    }else{
        Debug_Print((void *)a, 0, "Error Debug: ", COLOR_RED, TRUE);
        printf("\n");
        Fatal("Hash func for type not found", a->type.of);
        return 0;
    }
}

boolean Hashed_LocalEquals(MemCtx *m, Hashed *a, Hashed *b){
    if(a->id != b->id){
        return FALSE;
    }
    printf("LocalEquals %d\n", (m->type.state & LOCAL_PTR) != 0);
    return Abs_Eq(MemLocal_Trans(m, a->item), (void *)MemLocal_Trans(m, b->item));
}

boolean Hashed_ExternalEquals(MemCtx *m, Hashed *a, Hashed *b){
    if(a->id != b->id){
        return FALSE;
    }
    return Abs_Eq(a->item, (void *)MemLocal_Trans(m, b->item));
}


boolean Hashed_Equals(Hashed *a, Hashed *b){
    if(a->id != b->id){
        return FALSE;
    }
    return Abs_Eq(a->item, (void *)b->item);
}

Hashed *Hashed_Make(MemCtx *m, Abstract *a){
    if(a->type.of == TYPE_HASHED){
        return (Hashed *)a;
    }
    Hashed *v = (Hashed *)MemCtx_Alloc(m, sizeof(Hashed));
    v->type.of = TYPE_HASHED;
    v->id = Get_Hash(a);
    if((m->type.state & a->type.state & LOCAL_PTR) != 0){
        v->type.state |= LOCAL_PTR;
        LocalPtr lp;
        if((MemLocal_GetLocal(m, a, &lp) & SUCCESS) != 0){
            void **lptr = (void **)&lp;
            v->item = (Abstract *)*lptr;
        }else{
            Fatal("Error Getting Local Addr", TYPE_MEMCTX);
        }
    }else{
        v->item = a;
    }
    return v;
}

status Hash_Init(MemCtx *m){
    if(HashChain == NULL){
        Lookup *funcs = Lookup_Make(m, _TYPE_START, populateHash, NULL);
        HashChain = Chain_Make(m, funcs);
        return SUCCESS;
    }
    return NOOP;
}
