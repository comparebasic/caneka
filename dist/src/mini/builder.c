#include <external.h>
#include "./cnk.c"

static int REBUILD_ALL = FALSE;
static int REBUILD_BINARY = FALSE;
static Cstr lib_cstr;

struct builder {
    struct {
        uint16_t of;
        uint16_t state;
    } type;
    char *srcDir;
    char *buildDir;
} GlobalBuilder;

/* build functions */
static int Cstr_AddBuildName(Cstr *cstr, char *fname){
    Cstr_Add(cstr, fname);
    if(cstr->content[cstr->length-2] == '.' && cstr->content[cstr->length-1] == 'c'){
        cstr->content[cstr->length-1] = 'o';
        return TRUE;
    }
    return FALSE;
}

static int BuildObj(char *objName, StrArr *cmd_arr){
    return subProcess(cmd_arr, objName);
}

static int BuildSource(char *binary, char *fname, char *subdir){
    StrArr arr;

    Cstr build_cstr;
    Cstr_Init(&build_cstr, GlobalBuilder.buildDir);
    Cstr_Add(&build_cstr, "/");
    Cstr_Add(&build_cstr, subdir);
    Cstr_Add(&build_cstr, "/");
    Cstr_AddBuildName(&build_cstr, fname);

    Cstr source_cstr;
    Cstr_Init(&source_cstr, GlobalBuilder.srcDir);
    Cstr_Add(&source_cstr, "/");
    Cstr_Add(&source_cstr, subdir);
    Cstr_Add(&source_cstr, "/");
    Cstr_Add(&source_cstr, fname);

    if(SourceUpdated(&source_cstr, &build_cstr)){
        REBUILD_BINARY = TRUE;
        Arr_Init(&arr, CC);
        Arr_AddArr(&arr, CFLAGS);
        Arr_AddArr(&arr, INC);
        Arr_Add(&arr, "-c");
        Arr_Add(&arr, "-o");
        Arr_Add(&arr, build_cstr.content);
        Arr_Add(&arr, source_cstr.content);

        if(VERBOSE){
            printf("\x1b[%dmBuilding file %s -> %s\x1b[0m\n", MSG_COLOR, source_cstr.content, build_cstr.content);
        }

        if(BuildObj(source_cstr.content, &arr)){
            Arr_Init(&arr, AR);
            Arr_Add(&arr, "-rc");
            Arr_Add(&arr, lib_cstr.content);
            Arr_Add(&arr, build_cstr.content);

            return subProcess(&arr, build_cstr.content);
        }
    }
    return FALSE;
}

static int BuildBinary(char *binaryName, char *sourceName){
    StrArr arr;

    Cstr binary_cstr;
    Cstr_Init(&binary_cstr, GlobalBuilder.buildDir);
    Cstr_Add(&binary_cstr, "/");
    Cstr_Add(&binary_cstr, binaryName);

    Cstr source_cstr;
    Cstr_Init(&source_cstr, GlobalBuilder.srcDir);
    Cstr_Add(&source_cstr, "/");
    Cstr_Add(&source_cstr, sourceName);

    int updated = SourceUpdated(&source_cstr, &binary_cstr);

    if(!REBUILD_BINARY && !updated){
        return FALSE;
    }

    printf("\x1b[%dmBuilding binary %s\x1b[0m\n", MSG_COLOR, binaryName);

    Arr_Init(&arr, CC);
    Arr_AddArr(&arr, CFLAGS);
    Arr_AddArr(&arr, INC);
    Arr_Add(&arr, "-o");
    Arr_Add(&arr, binary_cstr.content);
    Arr_Add(&arr, source_cstr.content);
    Arr_Add(&arr, lib_cstr.content);
    Arr_AddArr(&arr, LIBS);

    return subProcess(&arr, binary_cstr.content);
}

int BuildLib(){
    Cstr_Init(&lib_cstr, GlobalBuilder.buildDir);
    Cstr_Add(&lib_cstr, "/");
    Cstr_Add(&lib_cstr, LIBTARGET);
    Cstr_Add(&lib_cstr, ".a");

    BuildSubdir **set = ALL;
    while(*set != NULL){
        BuildSubdir *dir = *set;
        FolderMake(dir->name);
        char **fname = dir->sources;
        while(*fname != NULL){
            BuildSource(LIBTARGET, *fname, dir->name);
            fname++;
        }
        set++;
    }
    return TRUE;
}

int Build(){
    if(BuildLib()){
        Target *target = TARGETS;
        int r = FALSE;
        while(target->bin != NULL){
            BuildBinary(target->bin, target->src);
            target++;
        }
        return r;
    }
    return FALSE;
}

int Clean(){
    BuildSubdir **set = ALL;
    char *name = "cleaning source objects"; 
    Cstr path;
    StrArr rm;
    while(*set != NULL){
        Cstr_Init(&path, GlobalBuilder.buildDir);
        Cstr_Add(&path, "/");
        Cstr_Add(&path, (*set)->name);

        Arr_Init(&rm, RM);
        Arr_Add(&rm, "-rfv");
        Arr_Add(&rm, path.content);
        set++;
        if(!subProcess(&rm, name)){
            return FALSE;
        }
    }


    Target *target = TARGETS;
    while(target->bin != NULL){
        Arr_Init(&rm, RM);
        Arr_Add(&rm, "-rfv");
        Cstr_Init(&path, GlobalBuilder.buildDir);
        Cstr_Add(&path, "/");
        Cstr_Add(&path, target->bin);
        Arr_Add(&rm, path.content);
        if(!subProcess(&rm, name)){
            return FALSE;
        }
        target++;
    }

    Arr_Init(&rm, RM);
    Arr_Add(&rm, "-rfv");
    Cstr_Init(&path, GlobalBuilder.buildDir);
    Cstr_Add(&path, "/");
    Cstr_Add(&path, LIBTARGET);
    Cstr_Add(&path, ".a");
    Arr_Add(&rm, path.content);
    if(!subProcess(&rm, name)){
        return FALSE;
    }

    return TRUE;
}

void **cmdSet[] = {
    (void *)"clean", (void *)Clean,
    (void *)"static", (void *)BuildLib,
    (void *)"build", (void *)Build,
    NULL, NULL
};

/* main */
int main(int argc, char *argv[]){
    memset(&GlobalBuilder, 0, sizeof(GlobalBuilder));
    GlobalBuilder.srcDir = "dist/src";
    GlobalBuilder.buildDir = "build";
    if(argc > 1){
        int i = 0;
        for(int ai = 1; ai < argc; ai++){
            char *arg = argv[ai];
            for(; cmdSet[i] != NULL; i+=2){
                char *cmd = (char *)cmdSet[i];
                Anon func = (Anon)cmdSet[i+1];
                if(!strcmp(arg, cmd)){
                    printf("Running %s\n", cmd);
                    if(!func()){
                        break;
                    }
                }
            }
        }
    }else{
        Build();
    }
    printf("\x1b[%dmAll Done\x1b[0m\n", DONE_COLOR);
}
