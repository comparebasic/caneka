#include <external.h>
#include <caneka.h>

static Span *pathSeps = NULL;

status IoUtil_Annotate(MemCh *m, StrVec *path){
    if(pathSeps == NULL){
        MemCh_SetToBase(m);
        pathSeps = Span_Make(m);
        Span_Add(pathSeps, (Abstract *)B_Wrapped(m, (byte)'/', ZERO, MORE));
        Span_Add(pathSeps, (Abstract *)B_Wrapped(m, (byte)'.', ZERO, LAST));
        MemCh_SetFromBase(m);
    }
    return Path_Annotate(m, path, pathSeps);
}

status IoUtil_RemoveSeps(MemCh *m, StrVec *path){
    i32 idx = 0;
    Iter it;
    Iter_Init(&it, path->p);
    while((Iter_Next(&it) & END) == 0){
        Str *s = (Str *)Iter_Get(&it);
        if((s->type.state & (MORE|LAST)) == 0){
            Span_Set(path->p, idx++, (Abstract *)s);
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
        StrVec_Add(v, s);
        StrVec_Add(v, Str_CstrRef(m, "/"));
        StrVec_Add(v, path);
    }

    return v;
}

StrVec *IoUtil_AbsVec(MemCh *m){
    return StrVec_From(m, IoUtil_GetCwdPath(m, NULL));
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

boolean IoUtil_CmpUpdated(MemCh *m, Str *a, Str *b, Access *ac){
    DebugStack_Push(a, a->type.of);
    struct stat source_stat;
    struct stat build_stat;
    int r = 0;

    char *path_cstr = Str_Cstr(m, a);
    r = stat(path_cstr, &source_stat);
    if(r != 0){
        Abstract *args[] = {
            (Abstract *)a,
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

StrVec *IoUtil_Fname(MemCh *m, StrVec *path){
    Coord cr;
    if(Path_Around(m, path, LAST, &cr) & SUCCESS){
        return StrVec_CoordCopy(m, path, &cr);
    }
    return NULL;
}

StrVec *IoUtil_BasePath(MemCh *m, StrVec *path){
    Coord cr;
    if(Path_RangeOf(m, path, MORE, &cr) & SUCCESS){
        return StrVec_CoordCopy(m, path, &cr);
    }
    return NULL;
}
