#include <external.h>
#include "cnkbuild_module.h"

i32 main(int argc, char **argv){
    MemBook *cp = MemBook_Make(NULL);
    void *args[5];
    if(cp == NULL){
        Fatal(FUNCNAME, FILENAME, LINENUMBER, "MemBook created successfully", NULL);
    }

    MemCh *m = MemCh_Make();
    if(m == NULL){
        Fatal(FUNCNAME, FILENAME, LINENUMBER, "MemCh created successfully", NULL);
    }

    Caneka_InitBase(m);
    BuildCtx_ToSInit(m);
    DebugStack_Push(NULL, 0);

    CliArgs *cli = CliArgs_Make(argc, argv);

    Str *helpKey = K(m, "help");
    Str *noColorKey = K(m, "no-color");
    Str *quietKey = K(m, "quiet");
    Str *runKey = K(m, "run");
    Str *modulesKey = K(m, "module");
    Str *licenceKey = K(m, "licence");
    Str *versionKey = K(m, "version");
    Str *srcKey = K(m, "src");
    Str *typeKey = K(m, "type");
    Str *dirKey = K(m, "dir");
    Str *libDirKey = K(m, "libDirs");
    Str *srcPrefixKey = K(m, "src-prefix");

    Args_Add(cli, helpKey, NULL, ARG_OPTIONAL, Sv(m, "Show this help message."));
    Args_Add(cli, noColorKey, NULL, ARG_OPTIONAL,
        Sv(m, "Skip ansi color sequences in output."));

    Args_Add(cli, srcKey, NULL, ARG_MULTIPLE,
        Sv(m, "Source code files or directories to build."));

    Args_Add(cli, srcPrefixKey, S(m, "src"), ARG_DEFAULT,
        Sv(m, "Source code files prefix. The path before the module names."));

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
    ctx->input.buildDir = CliArgs_GetAbsPath(cli, dirKey);
    ctx->current.dest = StrVec_Copy(m, ctx->input.buildDir);
    ctx->dir = StrVec_Copy(m, ctx->input.buildDir);
    ctx->src = CliArgs_GetAbsPath(cli, srcPrefixKey);
    ctx->current.source = CliArgs_GetAbsPath(cli, srcPrefixKey);
    ctx->input.sources = CliArgs_Get(cli, srcKey);
    ctx->input.srcPrefix = prefix;
    ctx->input.totalSources = I32_Wrapped(m, 0);
    ctx->input.countSources = I32_Wrapped(m, 0);
    ctx->input.totalModules = I32_Wrapped(m, 0);
    ctx->input.countModules = I32_Wrapped(m, 0);
    ctx->input.totalModuleSources = I32_Wrapped(m, 0);
    ctx->input.countModuleSources = I32_Wrapped(m, 0);

    if(CliArgs_Get(cli, quietKey)){
        BuildCtx_SetQuiet(TRUE);
    }else{
        BuildCli_SetupStatus(ctx);
    }

    BuildCtx_Build(ctx);

    Out("^g.Build succeeded^0\n", NULL);

    CliArgs_Free(cli);

    DebugStack_Pop();
    return 0;
}
