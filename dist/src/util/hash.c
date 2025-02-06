#include <external.h>
#include <caneka.h>

Chain *HashChain;

static util _Hash_Bytes(byte *bt, size_t length, util h){
	util i = 0;
	byte c;
	while (i++ < length) {
		c = *bt;
		h = (h << 5) + h + (h << 9) + h + (h << 31) + h + (h << 49) + h + c;
        if(i > 110 && i < 115){
            printf("'%c'/%u - %lu:%lu\n", c,c, i, h);
        }
        bt++;
	}
    return h;
}


static util Hash_Bytes(byte *bt, size_t length){
	util h = 5381;
    return _Hash_Bytes(bt, length, h);
}

static util Hash_Ptr(void *ptr){
	util h = 5381;
    return _Hash_Bytes((byte *)&ptr, sizeof(void *), h);
}

static util Hash_Abstract(Abstract *a){
    return 0;
}

static util Hash_PatMatch(Abstract *a){
    return 0;
}

static util Hash_PatCharDef(Abstract *a){
    return 0;
}

static util Hash_String(Abstract *a){
    String *s = (String *)asIfc(a, TYPE_STRING);
	util h = 5381;
    while(s != NULL){
        h = _Hash_Bytes(s->bytes, s->length, h);
        s = String_Next(s);
    }
    return h;
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
    r |= Lookup_Add(m, lk, TYPE_PATMATCH, (void *)Hash_PatMatch);
    r |= Lookup_Add(m, lk, TYPE_PATCHARDEF, (void *)Hash_PatCharDef);
    r |= Lookup_Add(m, lk, TYPE_STRING_CHAIN, (void *)Hash_String);
    r |= Lookup_Add(m, lk, TYPE_STRING_FIXED, (void *)Hash_String);
    r |= Lookup_Add(m, lk, TYPE_STRING_FULL, (void *)Hash_String);
    r |= Lookup_Add(m, lk, TYPE_REQ, (void *)Hash_Req);
    r |= Lookup_Add(m, lk, TYPE_SLAB, (void *)Hash_Slab);
    r |= Lookup_Add(m, lk, TYPE_SPAN, (void *)Hash_Span);
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
        return Hash_Ptr((void *)a);
    }
}

boolean Hashed_LocalEquals(MemCtx *m, Hashed *a, Hashed *b){
    if(a->id != b->id){
        return FALSE;
    }
    return Abs_Eq(a->item, b->item);
}

boolean Hashed_ExternalEquals(MemCtx *m, Hashed *a, Hashed *b){
    if(a->id != b->id){
        return FALSE;
    }
    return Abs_Eq(a->item, b->item);
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
    if(m->type.range == -1 && (a->type.state & LOCAL_PTR) != 0){
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

Hashed *Hashed_Clone(MemCtx *m, Hashed *oh){
    Hashed *h = MemCtx_Realloc(m, sizeof(Hashed), oh, sizeof(Hashed));
    if(h->item != NULL && ((h->item = Clone(m, h->item)) == NULL)){
        return NULL;
    }
    if(h->value != NULL && ((h->value = Clone(m, h->item)) == NULL)){
        return NULL;
    }
    if(h->next != NULL && ((h->next = Hashed_Clone(m, h->next)) == NULL)){
        return NULL;
    }
    return h;
}


status Hash_Init(MemCtx *m){
    if(HashChain == NULL){
        Lookup *funcs = Lookup_Make(m, _TYPE_START, populateHash, NULL);
        HashChain = Chain_Make(m, funcs);
        return SUCCESS;
    }
    return NOOP;
}
