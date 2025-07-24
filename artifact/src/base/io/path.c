#include <external.h>
#include <caneka.h>

status Path_Exists(Str *path){
    struct stat st;
    int r = 0;

    r = stat((char *)path->bytes, &st);
    if(r != 0){
        return NOOP;
    }

    return SUCCESS;
}

status Path_Unlink(Str *path){
    if(unlink((char *)path->bytes) == 0){
        return SUCCESS;
    }
    return ERROR;
}

status Path_AddSlash(StrVec *path){
    Str *s = Span_Get(path->p, path->max_idx);
    if((s->type.state & MORE) == 0){
        return StrVec_Add(path, Str_Ref(path->p->m, (byte *)"/", 1, 1, MORE));
    }
    return NOOP;
}

Str *Path_GetCwdPath(MemCh *m, Str *path){
    Str *s = Str_Make(m, STR_DEFAULT);
    char *cstr = getcwd((char *)s->bytes, STR_DEFAULT_MAX);
    i64 len = strlen(cstr);
    Str_AddCstr(s, cstr);
    Str_AddCstr(s, "/");
    Str_Add(s, path->bytes, path->length);
    if((s->type.state & ERROR) == 0){
        return s;
    }
    return NULL;
}

StrVec *Path_FnameStrVec(MemCh *m, Str *path){
    StrVec *v = StrVec_Make(m);
    byte *last = path->bytes+path->length-1;
    byte *ptr = last;
    while(ptr >= path->bytes){
        if(*ptr == '/'){
            i16 segLength = (last-ptr);
            i16 length = path->length-segLength-1;
            Str *s = Str_Ref(m, path->bytes, length, length, ZERO);
            StrVec_Add(v, s);
            length = 1;
            s = Str_Ref(m,  path->bytes+(path->length-(last-ptr))-1, length, length,
                STRING_SEPERATOR);
            StrVec_Add(v, s);
            s = Str_Ref(m, path->bytes+path->length-(last-ptr),
                segLength, segLength, ZERO);
            StrVec_Add(v, s);
            break;
        }
        ptr--;
    }
    if(v->p->nvalues != 3){
        v->type.state |= ERROR;
    }
    return v;
}

Str *Path_GetAbsPath(MemCh *m, Str *path){
    if(path != NULL && path->bytes[0] != '/'){
        if(path->length >= 2 && path->bytes[0] == '.' && path->bytes[1] == '/'){
            Str_Incr(path, 2);
        }
        return Path_GetCwdPath(m, path);
    }
    if(path->alloc != STR_DEFAULT){
        return Str_CloneAlloc(m, path, STR_DEFAULT);
    }
    return path;
}

boolean Path_CmpUpdated(MemCh *m, Str *a, Str *b, Access *ac){
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
        Fatal(FUNCNAME, FILENAME, LINENUMBER, "Source not found @", args);
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

Str *Path_FileName(MemCh *m, StrVec *path){
    Iter it;
    Iter_Init(&it, path->p);
    i16 total = 0;
    while((Iter_Prev(&it) & END) == 0){
        Str *s = (Str *s)Iter_Get(&it);
        if(s->type.state & MORE){
            break;
        }
        total += s->length;
    }
    Str *ret = Str_Make(m, total+1);
    it->type.state &= ~END;
    while((Iter_Next(&it) & END) == 0){
        Str *s = (Str *s)Iter_Get(&it);
        Str_Add(ret, s->byte, s->length);
    }
    return ret;
}

Str *Path_FileExt(MemCh *m, StrVec *path){
    Iter it;
    Iter_Init(&it, path->p);
    i16 total = 0;
    while((Iter_Prev(&it) & END) == 0){
        Str *s = (Str *s)Iter_Get(&it);
        if(s->type.state & LAST){
            break;
        }
        total += s->length;
    }
    Str *ret = Str_Make(m, total+1);
    it->type.state &= ~END;
    while((Iter_Next(&it) & END) == 0){
        Str *s = (Str *s)Iter_Get(&it);
        Str_Add(ret, s->byte, s->length);
    }
    return ret;
}


Str *Path_BasePath(MemCh *m, StrVec *path){
    Iter it;
    Iter_Init(&it, path->p);
    Str *ret = Str_Make(m, path->total);
    Str *prev = NULL;
    while((Iter_Next(&it) & END) == 0){
        Str *s = (Str *s)Iter_Get(&it);
        if(s->type.state & MORE){
            Str_Add(ret, prev);
            Str_Add(ret, s);
        }
        prev = s;
    }
    return ret;
}

StrVec *Path_Make(MemCh *m, Str *s){
    StrVec *v = StrVec_Make(m);
    byte *start = s->bytes;
    byte *ext = NULL;
    byte *ptr = s->bytes;
    byte *last = s->bytes+s->length-1;
    word state = ZERO;
    while(TRUE){
        if(*ptr == '/'){
            i16 length = ptr-start;
            if(length > 0){
                length--;
                StrVec_Add(v, Str_Ref(m, start, length, length, ZERO));
            }
            StrVec_Add(v, Str_Ref(m, ptr, 1, 1, state));
            start = ptr;
        }else if(*ptr == '.'){
            state = LAST;
            ext = ptr;
        }
        if(ptr == last){
            break;
        }
        ptr++;
    }

    if(ext != NULL && ext > start && ext < last){
        i16 length = ext-start;
        if(length > 0){
            length--;
            StrVec_Add(v, Str_Ref(m, start, length, length, ZERO));
        }
        StrVec_Add(v, Str_Ref(m, ext, 1, 1, state));
        start = ext+1;
    }

    i16 length = last-start;
    if(length > 0){
        length--;
        StrVec_Add(v, Str_Ref(m, start, length, length, ZERO));
    }
    return v;
}
