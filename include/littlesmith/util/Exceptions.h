//
// Created by stefan on 8/10/22.
//

#pragma once
#include <cstdarg>
#include <stdexcept>
#include "littlesmith/util/Compat.h"

namespace littlesmith {
    template<typename TEx>
    TEx formatException(const char *format, ...);
    std::runtime_error formatRuntimeError(const char *format, ...);

    template<typename TEx>
    inline TEx formatException(const char *format, ...) {
        va_list args;
        va_start(args, format);
        char what[4096];
        vsprintf(what, format, args);
        va_end(args);
        return TEx(what);
    }

    inline std::runtime_error formatRuntimeError(const char *format, ...) {
        va_list args;
        va_start(args, format);
        char what[4096];
        vsprintf(what, format, args);
        va_end(args);
        return std::runtime_error(what);

    }
}