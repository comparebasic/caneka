#include <external.h>
#include "base_module.h"

static Span *pathSeps = NULL;

Str *IoUtil_PathSep(MemCh *m){
    return Str_Ref(m, (byte *)"/", 1, 1, STRING_COPY|MORE);
}

boolean IoUtil_IsStrAbs(Str *s){
    return s->bytes[0] == '/';
}

boolean IoUtil_IsAbs(StrVec *v){
    Str *s = Span_Get(v->p, 0);
    return IoUtil_IsStrAbs(s);
}

StrVec *IoUtil_GetExt(MemCh *m, StrVec *path){
    StrVec *v = StrVec_Make(m);
    Iter it;
    Iter_Init(&it, path->p);
    while((Iter_Prev(&it) & END) == 0){
        Str *s = Iter_Get(&it);
        if(s->type.state & MORE){
            Error(m, FUNCNAME, FILENAME, LINENUMBER,
                "Error found path sep before exte sep", NULL);
            return NULL;
        }else if (s->type.state & LAST){
            break;
        }else{
            StrVec_Add(v, s);
        }
    }

    return v;
}

status IoUtil_AddExt(MemCh *m, StrVec *path, Str *ext){
    Iter it;
    Iter_Init(&it, path->p);
    word flags = ZERO;
    while((Iter_Prev(&it) & END) == 0){
        Str *s = Iter_Get(&it);
        flags = s->type.state;
        if(flags & MORE){
            break;
        }else if (s->type.state & LAST){
            s->type.state = ZERO;
            s->bytes[0] = '_';
        }
    }

    StrVec_Add(path, Str_Ref(m, (byte *)".", 1, 1, STRING_COPY|LAST));
    StrVec_Add(path, ext);
    return SUCCESS;
}

status IoUtil_SwapExt(MemCh *m, StrVec *path, Str *ext){
    void *args[3];
    args[0] = path;
    args[1] = NULL;
    args[2] = NULL;
    Iter it;
    Iter_Init(&it, path->p);
    word flags = ZERO;
    while((Iter_Prev(&it) & END) == 0){
        Str *s = Iter_Get(&it);
        args[1] = s;
        flags = s->type.state;
        if(flags & MORE){
            Error(m, FUNCNAME, FILENAME, LINENUMBER,
                "Expected path sep of flag type LAST before path sep of MORE"
                " in & at @", args);
            return ERROR;
        }else if (s->type.state & LAST){
            break;
        }else{
            Str *s = Iter_Get(&it);
            Iter_Remove(&it);
            path->total -= s->length;
        }
    }

    if((flags & LAST) == 0){
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "Extension seperator not found in @ at @", args);
        return ERROR;
    }

    if(ext != NULL){
        Iter_Add(&it, ext);
        path->total += ext->length;
    }else{
        Str *s = Iter_Get(&it);
        path->total -= s->length;
        Iter_Remove(&it);
    }
    return SUCCESS;
}

Span *IoUtil_AbsCmdArr(MemCh *m, StrVec *v){
    Span *p = Span_Make(m);

    Iter it;
    Iter_Init(&it, v->p);
    while((Iter_Next(&it) & END) == 0){
        Str *s = Iter_Get(&it);
        if((s->type.state & (MORE|LAST)) == 0){
            if(it.idx == 0){
                Span_Add(p, IoUtil_GetAbsPath(m, s));
            }else{
                Span_Add(p, s);
            }
        }
    }
    return p;
}

StrVec *IoPath(MemCh *m, char *cstr){
    return IoPath_From(m, Str_FromCstr(m, cstr, STRING_COPY));
}

/* deprecated use IoPath_FromStr */
StrVec *IoPath_From(MemCh *m, Str *s){
    StrVec *v = StrVec_From(m, s);
    IoUtil_Annotate(m, v);
    return v;
}

StrVec *IoPath_FromStr(MemCh *m, Str *s){
    StrVec *v = StrVec_From(m, s);
    IoUtil_Annotate(m, v);
    return v;
}

status IoPath_Descendent(StrVec *orig, StrVec *compare){
    if(compare->p->nvalues < orig->p->nvalues){
        return NOOP;
    }

    Iter itO;
    Iter_Init(&itO, orig->p);
    Iter itC;
    Iter_Init(&itC, compare->p);
    while(((Iter_Next(&itO)|Iter_Next(&itC)) & END) == 0){
        Str *o = Iter_Get(&itO);
        Str *c = Iter_Get(&itC);
        if(!Equals(o, c)){
            break;
        }
    }

    if(itO.type.state & END){
        return SUCCESS;
    }else if(itO.idx > 0){
        return MORE;
    }else{
        return NOOP;
    }

}

StrVec *IoPath_FromVec(MemCh *m, StrVec *_v){
    StrVec *v = StrVec_Copy(m, _v);
    IoUtil_Annotate(m, v);
    return v;
}

StrVec *IoAbsPath(MemCh *m, char *cstr){
    return IoUtil_AbsVec(m, StrVec_From(m, Str_FromCstr(m, cstr, STRING_COPY)));
}

StrVec *IoUtil_AbsPathBuilder(MemCh *m, char *args[]){
    StrVec *v = StrVec_Make(m);
    char **cptr = args;
    while(*cptr != NULL){
        StrVec_Add(v, S(m, *cptr));
        cptr++;
    }

    return IoUtil_AbsVec(m, v);
}

status IoUtil_Annotate(MemCh *m, StrVec *path){
    return Path_Annotate(m, path, pathSeps);
}

StrVec *IoUtil_AbsVec(MemCh *m, StrVec *v){
    Str *first = (Str *)Span_Get(v->p, 0);
    if(v != NULL){
        if(first != NULL && first->bytes[0] == '/'){
            return StrVec_Copy(m, v);
        }

        if(first->length >= 2 && first->bytes[0] == '.' && first->bytes[1] == '/'){
            first = Str_Clone(m, first);
            Str_Incr(first, 2);
            v->total -= 2;
        }
    }

    StrVec *path = StrVec_Make(m);
    Str *s = Str_Make(m, STR_DEFAULT);
    char *cstr = getcwd((char *)s->bytes, STR_DEFAULT);
    s->length = strlen(cstr);
    StrVec_Add(path, s);
    if(s->length > 0 && s->bytes[s->length-1] != '/' &&
            first->length > 0 && first->bytes[0] != '/'){
        StrVec_Add(path, Str_Ref(m, (byte *)"/", 1, 2, MORE));
    }
    StrVec_AddVec(path, v);
    
    IoUtil_Annotate(m, path);
    return path;
}

status IoUtil_AddVec(MemCh *m, StrVec *path, StrVec *v){
    Str *last = Span_Get(path->p, path->p->max_idx);
    if(last != NULL && last->length > 0 && last->bytes[last->length-1] != '/'){
        StrVec_Add(path, Str_Ref(m, (byte *)"/", 1, 2, MORE));
    }

    IoUtil_Annotate(m, v);
    return StrVec_AddVec(path, v);
}

status IoUtil_RemoveSeps(MemCh *m, StrVec *path){
    i32 idx = 0;
    Iter it;
    Iter_Init(&it, path->p);
    while((Iter_Next(&it) & END) == 0){
        Str *s = (Str *)Iter_Get(&it);
        if((s->type.state & (MORE|LAST)) == 0){
            Span_Set(path->p, idx++, s);
        }
    }
    path->p->nvalues = idx;
    path->p->max_idx = idx-1;
    return SUCCESS;
}

status IoUtil_Exists(Str *path){
    struct stat st;
    int r = 0;

    r = stat((char *)path->bytes, &st);
    if(r != 0){
        return NOOP;
    }

    return SUCCESS;
}

status IoUtil_Unlink(Str *path){
    if(unlink((char *)path->bytes) == 0){
        return SUCCESS;
    }
    return ERROR;
}

status IoUtil_AddSlash(StrVec *path){
    Str *s = Span_Get(path->p, path->p->max_idx);
    if((s->type.state & MORE) == 0){
        return StrVec_Add(path, Str_Ref(path->p->m, (byte *)"/", 1, 1, MORE));
    }
    return NOOP;
}

Str *IoUtil_GetCwdPath(MemCh *m, Str *path){
    Str *s = Str_Make(m, STR_DEFAULT);
    char *cstr = getcwd((char *)s->bytes, STR_DEFAULT_MAX);
    i64 len = strlen(cstr);
    Str_AddCstr(s, cstr);
    Str_AddCstr(s, "/");
    if(path != NULL){
        Str_Add(s, path->bytes, path->length);
        if((s->type.state & ERROR) == 0){
            return s;
        }else{
            return NULL;
        }
    }
    return s;
}

StrVec *IoUtil_GetAbsVec(MemCh *m, Str *path){
    StrVec *v = StrVec_Make(m);
    if(path != NULL && path->bytes[0] != '/'){
        if(path->length >= 2 && path->bytes[0] == '.' && path->bytes[1] == '/'){
            Str_Incr(path, 2);
        }

        Str *s = Str_Make(m, STR_DEFAULT);
        char *cstr = getcwd((char *)s->bytes, STR_DEFAULT_MAX);
        s->length = strlen(cstr);
        Str_Add(s, (byte *)"/", 1);
        StrVec_Add(v, s);
        StrVec_Add(v, path);
    }

    return v;
}

Str *IoUtil_GetAbsPath(MemCh *m, Str *path){
    if(path != NULL && path->bytes[0] != '/'){
        if(path->length >= 2 && path->bytes[0] == '.' && path->bytes[1] == '/'){
            Str_Incr(path, 2);
        }
        return IoUtil_GetCwdPath(m, path);
    }

    if(path->alloc != STR_DEFAULT){
        return Str_CloneAlloc(m, path, STR_DEFAULT);
    }

    return path;
}

boolean IoUtil_CmpUpdated(MemCh *m, Str *a, Str *b){
    DebugStack_Push(a, a->type.of);
    struct stat source_stat;
    struct stat build_stat;
    int r = 0;

    char *path_cstr = Str_Cstr(m, a);
    r = stat(path_cstr, &source_stat);
    if(r != 0){
        void *args[] = {
            a,
            NULL
        };
        Error(m, FUNCNAME, FILENAME, LINENUMBER, "Source not found @", args);
        exit(1);
    }
    r = stat(Str_Cstr(m, b), &build_stat);
    time_t build_mtime = 0;
    if(r == 0){
        build_mtime = build_stat.st_mtime;
    }

    if(source_stat.st_mtime > build_mtime){
        DebugStack_Pop();
        return TRUE;
    }else{
        DebugStack_Pop();
        return FALSE;
    }
}

i32 IoUtil_BasePathAnchor(StrVec *path){
    Iter it;
    Iter_Init(&it, path->p);
    while((Iter_Prev(&it) & END) == 0){
        Str *s = Iter_Get(&it);
        if(s->type.state & MORE){
            return it.idx;
        }
    }
    return 0;
}

Str *IoUtil_FnameStr(MemCh *m, StrVec *path){
    Str *s = Str_Make(m, min(path->total, STR_DEFAULT));
    Iter it;
    Iter_Init(&it, path->p);
    word max = 0;
    while((Iter_Next(&it) & END) == 0){
        Str *_s = Iter_Get(&it);
        if(_s->type.state & MORE){
           s->length = 0;  
           continue;
        }
        Str_Add(s, _s->bytes, _s->length);
        if(s->length > max){
            max = s->length;
        }
    }
    if(s->length < max){
        memset(s->bytes+s->length, 0, max - s->length);
    }
    return s;
}

StrVec *IoUtil_BasePath(MemCh *m, StrVec *path){
    return StrVec_CopyTo(m, path, IoUtil_BasePathAnchor(path));
}

status IoUtils_Init(MemCh *m){
    if(pathSeps == NULL){
        m->type.state |= MEMCH_BASE;
        pathSeps = Span_Make(m);
        Span_Add(pathSeps, B_Wrapped(m, (byte)'/', ZERO, MORE));
        Span_Add(pathSeps, B_Wrapped(m, (byte)'.', ZERO, LAST));
        m->type.state &= ~MEMCH_BASE;
        return SUCCESS;
    }
    return NOOP;
}
