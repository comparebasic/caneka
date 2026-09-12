#ifndef BUILDER_H
#define BUILDER_H

#define BUILDER_READ_SIZE 512

#include "detect.h"
#include <base_module.h>

enum build_types {
    BUILD_EXEC = 1 << 8,
    BUILD_STATIC = 1 << 9,
    BUILD_LINK = 1 << 10,
    BUILD_SKIP = 1 << 11,
    BUILD_CHOICE = 1 << 12,
    BUILD_INCLUDE = 1 << 13,
    BUILD_SHARED = 1 << 14,
    BUILD_SUB_DEP = 1 << 15,
};

#include "types/range.h"
#include "types/structs.h"
#include "buildctx.h"
#include "module.h"
#include "buildlogger.h"
#include "cli.h"
#include "object.h"
#include "buildeka_tos.h"

#endif
