#include "ely/assert.h"

#include <cstdio>
#include <exception>

#include <execinfo.h>
#include <unistd.h>

extern "C" const char* __progname;

void _ely_assert_fail(const char*  assertion,
                      const char*  msg,
                      const char*  file,
                      unsigned int line,
                      const char*  fn)
{
    fprintf(stderr,
            "%s%s%s:%u: %s%sAssertion `%s' failed: %s\n",
            __progname,
            __progname[0] ? ": " : "",
            file,
            line,
            fn ? fn : "",
            fn ? ": " : "",
            assertion,
            msg);
    fflush(stderr);

    static constexpr std::size_t frames_capacity = 512;
    void*                        stack_frames[512];

    std::size_t frames_size = backtrace(stack_frames, frames_capacity);
    std::fprintf(stderr,
                 "backtrace: %u frames\n",
                 static_cast<unsigned int>(frames_size));
    backtrace_symbols_fd(stack_frames, frames_size, STDERR_FILENO);
    fflush(stderr);
    std::terminate();
}

void _ely_unimplemented(const char*  msg,
                        const char*  file,
                        unsigned int line,
                        const char*  fn)
{
    fprintf(stderr,
            "%s%s%s:%u: %s%sis unimplemented: %s\n",
            __progname,
            __progname[0] ? ": " : "",
            file,
            line,
            fn ? fn : "",
            fn ? ": " : "",
            msg);
    fflush(stderr);
    std::terminate();
}