#include "mini/cnk.c"

static char *srcDir = "src";
static char *distDir = "dist";

int transDir(Cstr *dir_cstr){
    Cstr dist_cstr;
    Cstr_Init(&dist_cstr, distDir);
    Cstr_Add(&dist_cstr, "/");
    Cstr_Add(&dist_cstr, dir_cstr->content); 

    DIR* dir = opendir(dist_cstr.content);
    if(dir){
        closedir(dir);
    }else if(ENOENT == errno){
        printf("\x1b[%dmMaking Dist Directory %s\x1b[0m\n", MSG_COLOR, dist_cstr.content);
        StrArr mkdir;
        Arr_Init(&mkdir, MKDIR);
        Arr_Add(&mkdir, "-p");
        Arr_Add(&mkdir, dist_cstr.content);
        if(!subProcess(&mkdir, "Folder Make")){
            return FALSE;
        }
    }
    return TRUE;
}

static int transpile(Cstr *source, Cstr *dest){
    printf("\x1b[1;34m%s -> %s\x1b[0m\n", source->content, dest->content);
    return TRUE;
}

static int copy(Cstr *source, Cstr *dest){
    /*
    printf("\x1b[33m%s -> %s\x1b[0m\n", source->content, dest->content);
    */
    return TRUE;
}

int transFile(Cstr *dir_cstr, Cstr *file_cstr){
    char *C = ".c";
    char *Cnk = ".cnk";
    char *H = ".h";

    Cstr source;
    Cstr_Init(&source, dir_cstr->content);
    Cstr_Add(&source, "/");
    Cstr_Add(&source, file_cstr->content);

    Cstr dest;
    Cstr_Init(&dest, distDir);
    Cstr_Add(&dest, "/");
    Cstr_Add(&dest, dir_cstr->content);
    Cstr_Add(&dest, "/");
    Cstr_Add(&dest, file_cstr->content);

    if(Cstr_Match(file_cstr->content, C, MATCH_END) || Cstr_Match(file_cstr->content, H, MATCH_END)){
        copy(&source, &dest);
    }else if(Cstr_Match(file_cstr->content, Cnk, MATCH_END)){
        int l = strlen(dest.content);
        dest.content[l-2] = '\0';
        transpile(&source, &dest);
    }else{
        copy(dir_cstr, file_cstr);
    }

    return TRUE;
}



void Trans(){
    Cstr dir_cstr;
    Cstr_Init(&dir_cstr, srcDir);
    FolderClimb(&dir_cstr, transDir, transFile);
}
