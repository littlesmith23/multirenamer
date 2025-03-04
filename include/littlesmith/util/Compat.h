//
// Created by stefan on 8/10/22.
//

#pragma once
#include <cstdarg>
#include <cstdio>
#include <cstdint>
#include <string>
#define UNREFERENCED_PARAMETER(x) (void)(x)
#ifndef _WIN32
#ifndef strcpy_s
#define strcpy_s(dst, len, src) strcpy(dst, src)
#endif
#define UNREFERENCED __attribute__((unused))
#define MAYBE_UNUSED [[maybe_unused]]
int sprintf_s(char* dst, int len, const char* fmt, ...);

inline int sprintf_s(char* dst, int len, const char* fmt, ...) {
    UNREFERENCED_PARAMETER(len);
    va_list args;
    va_start(args, fmt);
    int32_t ret = vsprintf(dst, fmt, args);
    va_end(args);
    return ret;
}
#define LINEFEED "\n"
#else
#define LINEFEED "\r\n"
#define UNREFERENCED
#define MAYBE_UNUSED
#endif
