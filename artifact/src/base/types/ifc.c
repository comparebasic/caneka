#include <external.h>
#include <caneka.h>

Lookup *SizeLookup = NULL;

static status setSizeLookup(MemCh *m, Lookup *lk){
    status r = READY;

    r |= Lookup_AddRaw(m, lk, TYPE_WRAPPED, (i64)sizeof(Single));
    r |= Lookup_AddRaw(m, lk, TYPE_WRAPPED_FUNC, (i64)sizeof(Single));
    r |= Lookup_AddRaw(m, lk, TYPE_WRAPPED_DO, (i64)sizeof(Single));
    r |= Lookup_AddRaw(m, lk, TYPE_WRAPPED_UTIL, (i64)sizeof(Single));
    r |= Lookup_AddRaw(m, lk, TYPE_WRAPPED_MEMCOUNT, (i64)sizeof(Single));
    r |= Lookup_AddRaw(m, lk, TYPE_WRAPPED_I64, (i64)sizeof(Single));
    r |= Lookup_AddRaw(m, lk, TYPE_WRAPPED_I32, (i64)sizeof(Single));
    r |= Lookup_AddRaw(m, lk, TYPE_WRAPPED_I16, (i64)sizeof(Single));
    r |= Lookup_AddRaw(m, lk, TYPE_WRAPPED_I8, (i64)sizeof(Single));
    r |= Lookup_AddRaw(m, lk, TYPE_WRAPPED_TIME64, (i64)sizeof(Single));
    r |= Lookup_AddRaw(m, lk, TYPE_WRAPPED_BOOL, (i64)sizeof(Single));
    r |= Lookup_AddRaw(m, lk, TYPE_WRAPPED_PTR, (i64)sizeof(Single));
    r |= Lookup_AddRaw(m, lk, TYPE_WRAPPED_CSTR, (i64)sizeof(Single));
    r |= Lookup_AddRaw(m, lk, TYPE_BOOK, (util)sizeof(MemBook));
    r |= Lookup_AddRaw(m, lk, TYPE_MEMCTX, (util)sizeof(MemCh));
    r |= Lookup_AddRaw(m, lk, TYPE_MEMSLAB, (util)sizeof(MemPage));
    r |= Lookup_AddRaw(m, lk, TYPE_MESS, (util)sizeof(Mess));
    r |= Lookup_AddRaw(m, lk, TYPE_MESS_CLIMBER, (util)sizeof(MessClimber));
    r |= Lookup_AddRaw(m, lk, TYPE_NODE, (util)sizeof(Node));
    r |= Lookup_AddRaw(m, lk, TYPE_STR, (util)sizeof(Str));
    r |= Lookup_AddRaw(m, lk, TYPE_STRVEC, (util)sizeof(StrVec));
    r |= Lookup_AddRaw(m, lk, TYPE_STREAM, (util)sizeof(Stream));
    r |= Lookup_AddRaw(m, lk, TYPE_STREAM_TASK, (util)sizeof(StreamTask));
    r |= Lookup_AddRaw(m, lk, TYPE_FMT_LINE, (util)sizeof(FmtLine));
    r |= Lookup_AddRaw(m, lk, TYPE_CURSOR, (util)sizeof(Cursor));
    r |= Lookup_AddRaw(m, lk, TYPE_TESTSUITE, (util)sizeof(TestSet));
    r |= Lookup_AddRaw(m, lk, TYPE_SNIP, (util)sizeof(Snip));
    r |= Lookup_AddRaw(m, lk, TYPE_SNIPSPAN, (util)sizeof(Span));
    r |= Lookup_AddRaw(m, lk, TYPE_ROEBLING, (util)sizeof(Roebling));
    r |= Lookup_AddRaw(m, lk, TYPE_PATMATCH, (util)sizeof(Match));
    /*
    r |= Lookup_AddRaw(m, lk, TYPE_IOCTX, (util)TYPE_IOCTX);
    r |= Lookup_AddRaw(m, lk, TYPE_ACCESS, (util)TYPE_ACCESS);
    r |= Lookup_AddRaw(m, lk, TYPE_PERMISSION, (util)TYPE_PERMISSION);
    r |= Lookup_AddRaw(m, lk, TYPE_AUTH, (util)TYPE_AUTH);
    r |= Lookup_AddRaw(m, lk, TYPE_SPAN, (util)TYPE_SPAN);
    r |= Lookup_AddRaw(m, lk, TYPE_TABLE, (util)TYPE_TABLE);
    r |= Lookup_AddRaw(m, lk, TYPE_RELATION, (util)TYPE_RELATION);
    r |= Lookup_AddRaw(m, lk, TYPE_ARRAY, (util)TYPE_ARRAY);
    r |= Lookup_AddRaw(m, lk, TYPE_HKEY, (util)TYPE_HKEY);
    r |= Lookup_AddRaw(m, lk, TYPE_ORDERED_TABLE, (util)TYPE_ORDERED_TABLE);
    r |= Lookup_AddRaw(m, lk, TYPE_COORDS, (util)TYPE_COORDS);
    r |= Lookup_AddRaw(m, lk, TYPE_SLAB, (util)TYPE_SLAB);
    r |= Lookup_AddRaw(m, lk, TYPE_HASHED, (util)TYPE_HASHED);
    r |= Lookup_AddRaw(m, lk, TYPE_MEMPAIR, (util)TYPE_MEMPAIR);
    r |= Lookup_AddRaw(m, lk, TYPE_FILE, (util)TYPE_FILE);
    r |= Lookup_AddRaw(m, lk, TYPE_SPOOL, (util)TYPE_SPOOL);
    r |= Lookup_AddRaw(m, lk, TYPE_LOOKUP, (util)TYPE_LOOKUP);
    r |= Lookup_AddRaw(m, lk, TYPE_PROCDETS, (util)TYPE_PROCDETS);
    r |= Lookup_AddRaw(m, lk, TYPE_ITER, (util)TYPE_ITER);
    r |= Lookup_AddRaw(m, lk, TYPE_SINGLE, (util)TYPE_SINGLE);
    r |= Lookup_AddRaw(m, lk, TYPE_TRANSP, (util)TYPE_TRANSP);
    r |= Lookup_AddRaw(m, lk, TYPE_TOKENIZE, (util)TYPE_TOKENIZE);
    r |= Lookup_AddRaw(m, lk, TYPE_DEBUG_STACK, (util)TYPE_DEBUG_STACK);
    r |= Lookup_AddRaw(m, lk, TYPE_DEBUG_STACK_ENTRY, (util)TYPE_DEBUG_STACK_ENTRY);
    r |= Lookup_AddRaw(m, lk, TYPE_TARGET, (util)TYPE_TARGET);
    r |= Lookup_AddRaw(m, lk, TYPE_SOURCE, (util)TYPE_SOURCE);
    r |= Lookup_AddRaw(m, lk, TYPE_PROC_IO_SET, (util)TYPE_PROC_IO_SET);
    r |= Lookup_AddRaw(m, lk, TYPE_SERVECTX, (util)TYPE_SERVECTX);
    r |= Lookup_AddRaw(m, lk, TYPE_HANDLER, (util)TYPE_HANDLER);
    r |= Lookup_AddRaw(m, lk, TYPE_REQ, (util)TYPE_REQ);
    r |= Lookup_AddRaw(m, lk, TYPE_PROTO, (util)TYPE_PROTO);
    r |= Lookup_AddRaw(m, lk, TYPE_PROTODEF, (util)TYPE_PROTODEF);
    r |= Lookup_AddRaw(m, lk, TYPE_IO_PROTO, (util)TYPE_IO_PROTO);
    r |= Lookup_AddRaw(m, lk, TYPE_IO_PROTODEF, (util)TYPE_IO_PROTODEF);
    r |= Lookup_AddRaw(m, lk, TYPE_SUB_PROTO, (util)TYPE_SUB_PROTO);
    r |= Lookup_AddRaw(m, lk, TYPE_SUB_PROTODEF, (util)TYPE_SUB_PROTODEF);
    r |= Lookup_AddRaw(m, lk, TYPE_STEP, (util)TYPE_STEP);
    r |= Lookup_AddRaw(m, lk, TYPE_SECURE_PRIVKEY, (util)TYPE_SECURE_PRIVKEY);
    r |= Lookup_AddRaw(m, lk, TYPE_SECURE_PUBKEY, (util)TYPE_SECURE_PUBKEY);
    */

    return r;
}

void Type_SetFlag(Abstract *a, word flags){
    a->type.state = (a->type.state & NORMAL_FLAGS) | flags;
}

cls Ifc_Get(cls inst){
    if(inst == TYPE_SPAN || inst == TYPE_TABLE){
        return TYPE_SPAN;
    }

    return inst;
}

boolean Ifc_Match(cls inst, cls ifc){
    if(inst == ifc){
        return TRUE;
    }

    if(ifc == TYPE_MEMCTX){
        return inst == TYPE_SPAN || inst == TYPE_REQ || inst == TYPE_SERVECTX 
            || inst == TYPE_ROEBLING;
    if(ifc == TYPE_MEMLOCAL_ITEM){
        return inst >= HTYPE_LOCAL;
    }
    }else if(ifc == TYPE_WRAPPED){
        return (inst == TYPE_WRAPPED_DO || inst == TYPE_WRAPPED_UTIL ||
            inst == TYPE_WRAPPED_FUNC || inst == TYPE_WRAPPED_PTR ||
            inst == TYPE_WRAPPED_I64 || inst == TYPE_WRAPPED_I32 ||
            inst == TYPE_WRAPPED_I16 || inst == TYPE_WRAPPED_I8);
    }else if(ifc == TYPE_SPAN){ 
        return (inst == TYPE_SPAN || inst == TYPE_TABLE); 
    /*
    }else if(ifc == TYPE_TRANSP){ 
        return (inst == TYPE_TRANSP || inst == TYPE_LANG_CNK);
    */
    }else if(ifc == TYPE_TABLE){ 
        return (inst == TYPE_TABLE || inst == TYPE_ORDERED_TABLE);
    /*
    }else if(ifc == TYPE_FMT_CTX){ 
        return (inst == TYPE_FMT_CTX || inst == TYPE_LANG_CNK || inst == TYPE_LANG_CDOC);
    */
    }

    return FALSE;
}

status Ifc_Init(MemCh *m){
    if(SizeLookup == NULL){
        SizeLookup = Lookup_Make(m, _TYPE_ZERO, NULL, NULL);
        return setSizeLookup(m, SizeLookup);
    }
    return NOOP;
}
