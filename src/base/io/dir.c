/* Base.io.Dir
 *
 * Functions for managing directories.
 *
 * The DirSelector object is used to query and manage files in a directory
 */

#include <external.h>
#include "base_module.h"

static Str _globalS;
static byte _buff[STR_DEFAULT];

Span *FilePathSep = NULL;

static status fileHasExt(MemCh *m, void *_file, void *source){
    Str *file = _file;
    Span *exts = (Span *)as(source, TYPE_SPAN);
    Iter it;
    Iter_Init(&it, exts);
    while((Iter_Next(&it) & END) == 0){
        Str *e = Iter_Get(&it); 
        if(file->length >= e->length+1 &&
                file->bytes[file->length-(e->length+1)] == '.' && Str_EndMatch(file, e)){
            return SUCCESS;
        }
    }
    return NOOP;
}

static status fnameStr(MemCh *m, Str *s, Str *path, Str *file){
    Str_Add(s, path->bytes, path->length);
    if(path->length > 0 && path->bytes[path->length-1] != '/'){
        Str_Add(s, (byte *)"/", 1);
    }
    Str_Add(s, file->bytes, file->length);
    return SUCCESS;
}

static status rmDir(MemCh *m, Str *path, void *source){
    char *dirPath = Str_Cstr(m, path);
    return rmdir(dirPath) == 0 ? SUCCESS : ERROR;
}

static status rmFile(MemCh *m, Str *path, Str *file, void *source){
    Str *s = &_globalS;
    Str_Init(s, _buff, STR_DEFAULT, STR_DEFAULT);
    fnameStr(m, s, path, file);
    char *rmPath = Str_Cstr(m, s);
    return unlink(rmPath) == 0 ? SUCCESS : ERROR;
}

static status gatherDir(MemCh *m, Str *path, void *source){
    Span *p = (Span *)as(source, TYPE_SPAN);
    StrVec *v = StrVec_From(m, path);
    v->type.state |= MORE;
    return Span_Add(p, v);
}

static status gatherFile(MemCh *m, Str *path, Str *file, void *source){
    Span *p = (Span *)asIfc(source, TYPE_SPAN);
    StrVec *v = StrVec_Make(m);
    StrVec_Add(v, path);
    if(path->bytes[path->length-1] != '/'){
        StrVec_Add(v, Str_Ref(m, (byte *)"/", 1, 1, 0));
    }
    StrVec_Add(v, file);
    return Span_Add(p, v);
}

static status gatherFileFiltered(MemCh *m, Str *path, Str *file, void *source){
    DirSelector *sel = (DirSelector *)source;
    if(sel->func(m, file, sel->source) & SUCCESS){
        Span *p = (Span *)asIfc(sel->dest, TYPE_SPAN);
        StrVec *v = StrVec_Make(m);
        StrVec_Add(v, path);
        if(path->bytes[path->length-1] != '/'){
            StrVec_Add(v, Str_Ref(m, (byte *)"/", 1, 1, 0));
        }
        StrVec_Add(v, file);
        return Span_Add(p, v);
    }
    return NOOP;
}

static status gatherDirSel(MemCh *m, Str *path, void *source){
    DirSelector *sel = (DirSelector *)as(source, TYPE_DIR_SELECTOR);
    Span *p = sel->dest;
    StrVec *v = StrVec_From(m, path);
    v->type.state |= MORE;
    return Span_Add(p, v);
}

static status gatherFileSel(MemCh *m, Str *path, Str *file, void *source){
    DirSelector *sel = (DirSelector *)as(source, TYPE_DIR_SELECTOR);
    status r = READY;
    StrVec *v = StrVec_Make(m);
    StrVec_Add(v, path);
    if(path->bytes[path->length-1] != '/'){
        StrVec_Add(v, Str_Ref(m, (byte *)"/", 1, 1, 0));
    }
    StrVec_Add(v, file);

    boolean extMatches = FALSE;
    if(file->length >= sel->ext->length){
        Str ext = {
            .type = {TYPE_STR, STRING_CONST},
            .length = sel->ext->length,
            .alloc = sel->ext->length,
            .bytes = file->bytes+(file->length - sel->ext->length)
        };
        if(Equals(&ext, sel->ext)){
            extMatches = TRUE;
        }
    }

    if(extMatches || (sel->type.state & DIR_SELECTOR_MTIME_ALL)){
        struct stat st;
        File_Stat(m, StrVec_Str(m, v), &st);
        if((sel->type.state & DIR_SELECTOR_MTIME_LOWEST)){
            if(sel->time.tv_sec > st.st_mtime){
                sel->time.tv_sec = st.st_mtime;
                sel->time.tv_nsec = 0;
            }
        }else{
            if(sel->time.tv_sec < st.st_mtime){
                sel->time.tv_sec = st.st_mtime;
                sel->time.tv_nsec = 0;
                r |= PROCESSING;
            }
        }
    }

    if(extMatches){
        Span_Add(sel->dest, v);
        r |= SUCCESS;
    }

    return r;
}

status Dir_Rm(MemCh *m, Str *path){
    char *dirPath = Str_Cstr(m, path);
    return rmdir(dirPath) == 0 ? SUCCESS : ERROR;
}

status Dir_Destroy(MemCh *m, Str *path){
    status r = READY;
    r |= Dir_Climb(m, path, rmDir, rmFile, NULL);
    r |= rmDir(m, path, NULL);
    return r;
}

status Dir_Gather(MemCh *m, Str *path, Span *sp){
    return Dir_Climb(m, path, gatherDir, gatherFile, sp);
}

DirSelector *Dir_GatherByExt(MemCh *m, Str *path, Span *sp, Span *exts){
    DirSelector *sel = DirSelector_Make(m,
        NULL, sp, DIR_SELECTOR_NODIRS|DIR_SELECTOR_FILTER);
    sel->func = fileHasExt;
    sel->source = exts;

    if((Dir_Climb(m, path, NULL, gatherFileFiltered, sel) & ERROR) == 0){
        return sel;
    }
    return NULL;
}

status Dir_GatherSel(MemCh *m, Str *path, DirSelector *sel){
    /* Gather all directories and files with the directory specified in *path
     *
     * path: path to climb
     * sel: object to place files and directory values found, loggin the
     *   lastest (or least) modified time in the *time* value of the *sel*
     *   object.
     */
    if(sel->type.state & DIR_SELECTOR_NODIRS){
        return Dir_Climb(m, path, NULL, gatherFileSel, sel);
    }else{
        return Dir_Climb(m, path, gatherDirSel, gatherFileSel, sel);
    }
}

status Dir_Exists(MemCh *m, Str *path){
    char *path_cstr = Str_Cstr(m, path);
    DIR* dir = opendir(path_cstr);
    if(dir){
        closedir(dir);
        return SUCCESS;
    }else if(ENOENT == errno){
        return NOOP;
    }
    return ERROR;
}

status Dir_Climb(MemCh *m, Str *path, DirFunc dir, FileFunc file, void *source){
    /* Dispatch files and directories to function pointers 
     *
     * path: directory path, local or absolute to start the directory climb.
     * dir: function called for each directory found
     * file: function called for each file found
     * source: Abstract object passed to each function
     */
    DebugStack_Push(path, path->type.of); 
    status r = READY;
    struct dirent *ent;
    DIR *d = opendir((char *)path->bytes);
    if(d != NULL){
        while((ent = readdir(d)) != NULL){
            if(ent->d_name[0] == '.' && (ent->d_name[0] == '.' || ent->d_name[0] == '0')){
                continue;
            }
            i64 len = strlen(ent->d_name);
            Str *e = Str_Ref(m, (byte *)ent->d_name, len, len+1, STRING_COPY);
            if(ent->d_type == IS_DIR){
                Str *s = Str_Make(m, STR_DEFAULT);
                fnameStr(m, s, path, e);
                if(dir != NULL){
                    r |= dir(m, s, source);
                }
                Dir_Climb(m, s, dir, file, source);
            }else{
                r |= file(m, path, e, source);
            }
        }
        closedir(d);
        DebugStack_Pop();
        return r|SUCCESS;
    }else{
        void *args[] = {path, NULL};
        Error(m, FUNCNAME, FILENAME, LINENUMBER,
            "Unable to open Direcotry: @", args);
        DebugStack_Pop();
        return r|ERROR;
    }
}

status Dir_Mk(MemCh *m, Str *path){
    if(mkdir(Str_Cstr(m, path), 0766) == 0){
        return SUCCESS;
    }else{
        return ERROR;
    }
}

DirSelector *DirSelector_Make(MemCh *m, Str *ext, Span *dest, word flags){
    DirSelector *sel = MemCh_AllocOf(m, sizeof(DirSelector), TYPE_DIR_SELECTOR);
    sel->type.of = TYPE_DIR_SELECTOR;
    sel->type.state = flags;
    if(dest == NULL){
        dest = Span_Make(m);
    }
    sel->meta = Table_Make(m);
    sel->ext = ext;
    sel->dest = dest;
    return sel;
}

status Dir_CheckCreate(MemCh *m, Str *path){
    /* 
     * :deprecated
     *
     * Create a directory and intermediate directories if it does not exist
     *
     * Using a subprocess for this is a bit much, it will be replaced
     * by more minimal POSIX functions at some point
     */
    DebugStack_Push(path, path->type.of);
    Span *cmd = Span_Make(m);
    char *cstr = "mkdir";
    i64 len = strlen(cstr);
    Span_Add(cmd, Str_Ref(m, (byte *)cstr, len, len+1, 0));
    cstr = "-p";
    len = strlen(cstr);
    Span_Add(cmd, Str_Ref(m, (byte *)cstr, len, len+1, 0));
    Span_Add(cmd, path);

    ProcDets pd;
    ProcDets_Init(&pd);
    if(path->type.state & DEBUG){
        cmd->type.state |= DEBUG;
    }
    status r = SubProcess(m, cmd, &pd);
    DebugStack_Pop();
    return r;
}
