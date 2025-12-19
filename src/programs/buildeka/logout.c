#include <external.h>
#include "buildeka_module.h"

boolean _quiet = FALSE;

void BuildCtx_SetQuiet(boolean quiet){
    _quiet = quiet;
}

status BuildCtx_Log(BuildCtx *ctx){
    if(_quiet){
        void *args[7];
        if(ctx->type.state & SUCCESS){
            args[0] = ctx->cli.fields.current[BUILIDER_CLI_LIBFILENAME];
            args[1] = ctx->input.totalModules,
            args[2] = NULL;
            Out("Static Library Complete $ $ modules", args);
        }else if(ctx->type.state & ERROR){
            args[0] = ctx->cli.fields.current[BUILIDER_CLI_LIBFILENAME];
            args[1] = NULL;
            Out("Error Building Static Library $\n", args);
        }else if(ctx->type.state & PROCESSING){
            if(ctx->type.state & NOOP){
                Out("Linking $ of $ ", args);
                void *_args[] = {
                    ctx->cli.fields.current[0],
                    ctx->cli.fields.current[2],
                    NULL
                };
                Out(" for $ $ $\n", _args);
            }else{
                args[0] = ctx->input.countModuleSources;
                args[1] = ctx->input.totalModuleSources;
                args[2] = ctx->input.countModules;
                args[3] = ctx->input.totalModules;
                args[4] = ctx->input.countSources;
                args[5] = ctx->input.totalSources;
                args[6] = NULL;
                Out("$/$ of module $/$ total $/$", args);
                Out(" for $\n    $ $ -> $\n", ctx->cli.fields.current);
            }
        }else{
            args[0] = ctx->input.countModules;
            args[1] = ctx->input.totalModules;
            args[2] = NULL;
            Out("Module $ of $:", args);
            Out("$ $ $ -> $\n", ctx->cli.fields.current);
        }
    }else{
        if(ctx->type.state & SUCCESS){
            BuildCli_SetupComplete(ctx);
            CliStatus_Print(OutStream, ctx->cli.cli);
            CliStatus_PrintFinish(OutStream, ctx->cli.cli);
        }else if(ctx->type.state & ERROR){
            CliStatus_PrintFinish(OutStream, ctx->cli.cli);
        }else{
            CliStatus_Print(OutStream, ctx->cli.cli);
        }
    }
    return ZERO;
}
