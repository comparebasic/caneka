#include <external.h>
#include "buildeka_module.h"

i32 main(int argc, char **argv){
    MemBook *cp = MemBook_Make(NULL);
    void *args[3];

    if(cp == NULL){
        Fatal(NULL, FUNCNAME, FILENAME, LINENUMBER, "MemBook created successfully", NULL);
    }

    MemCh *m = MemCh_Make();
    if(m == NULL){
        Fatal(NULL, FUNCNAME, FILENAME, LINENUMBER, "MemCh created successfully", NULL);
    }

    Caneka_InitBase(m);
    BuildCtx_ToSInit(m);

    CliArgs *cli = CliArgs_Make(argc, argv);
    Debug_Push(m, cli);

    Str *helpKey = K(m, "help");
    Str *noColorKey = K(m, "no-color");
    Str *quietKey = K(m, "quiet");
    Str *runKey = K(m, "run");
    Str *modulesKey = K(m, "module");
    Str *optionsKey = K(m, "option");
    Str *licenceKey = K(m, "licence");
    Str *versionKey = K(m, "version");
    Str *targetKey = K(m, "target");
    Str *typeKey = K(m, "type");
    Str *dirKey = K(m, "dir");
    Str *debugKey = K(m, "debug");
    Str *libDirKey = K(m, "libDirs");
    Str *srcPrefixKey = K(m, "src-prefix");

    Args_Add(cli, helpKey, NULL, ARG_OPTIONAL, Sv(m, "Show this help message."));
    Args_Add(cli, noColorKey, NULL, ARG_OPTIONAL,
        Sv(m, "Skip ansi color sequences in output."));

    Args_Add(cli, targetKey, NULL, ZERO,
        Sv(m, "Target program or module to build."));

    Args_Add(cli, srcPrefixKey, S(m, "src"), ARG_DEFAULT,
        Sv(m, "Source code files prefix. The path before the module names."));

    Args_Add(cli, optionsKey, NULL, ARG_MULTIPLE|ARG_OPTIONAL,
        Sv(m, "Optional dependency source modules to include."));

    Args_Add(cli, debugKey, NULL, ARG_OPTIONAL,
        Sv(m, "Optionaly add debug objects to objects."));

    Span *libDirs = Span_Make(m);
    Span_Add(libDirs, S(m, "/usr/lib64"));
    Args_Add(cli, libDirKey, libDirs, ARG_MULTIPLE|ARG_DEFAULT,
        Sv(m, "lib directories to source static or linked libraries from."));

    Args_Add(cli, dirKey, S(m, "build"), ARG_DEFAULT,
        Sv(m, "Build directory to use for objects and binary assets/executables."));

    Span *types = Span_Make(m);
    Span_Add(types, S(m, "exec"));
    Span_Add(types, S(m, "static"));
    Args_Add(cli, typeKey, types, ARG_CHOICE|ARG_DEFAULT,
        Sv(m, "Type of binary asset to create, static builds a static library,"
        " exec builds and executable."));

    Args_Add(cli, quietKey, NULL, ARG_OPTIONAL,
        Sv(m, "Output a list of compiled targets instead of the progress bar."));

    Args_Add(cli, runKey, NULL, ARG_OPTIONAL,
        Sv(m, "Run the binary after it is built."));

    Args_Add(cli, licenceKey, NULL, ARG_OPTIONAL,
        Sv(m, "Show build program intellectual property licence."));
        
    Args_Add(cli, versionKey, NULL, ARG_OPTIONAL,
        Sv(m, "Show build program version."));

    CliArgs_Parse(cli);

    BuildCtx *ctx = BuildCtx_Make(m);

    StrVec *prefix = StrVec_From(m, CliArgs_Get(cli, srcPrefixKey));
    IoUtil_Annotate(m, prefix);

    ctx->dest = CliArgs_GetAbsPath(cli, dirKey);
    Str *pathS = CliArgs_Get(cli, srcPrefixKey);
    Path_StrRmTrailingSlash(m, pathS);
    ctx->src = IoUtil_AbsVec(m, StrVec_From(m, pathS));
    StrVec *targetV = StrVec_From(m, CliArgs_Get(cli, targetKey));
    ctx->ident = Ident_FromVec(m, targetV);
    ctx->options = CliArgs_Get(cli, optionsKey);

    if(CliArgs_Get(cli, quietKey)){
        Ansi_SetColor(OutStream, FALSE);
    }

    ctx->current.flags = Span_Make(m);
    if(CliArgs_Get(cli, debugKey)){
        Span_Add(ctx->current.flags, Sv(m, "-g"));
    }

    BuildCtx_Setup(ctx);
    BuildCtx_ShowStatus(ctx);
    BuildCtx_Build(ctx);

    args[0] = CliArgs_Get(cli, targetKey);
    args[1] = CliArgs_Get(cli, dirKey);
    args[2] = NULL;
    Out("^g.Build succeeded $ -> ./$/bin/^0\n", args);

    CliArgs_Free(cli);

    Return(m, 0);
}
