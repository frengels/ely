#pragma once

#include "ely/defines.h"
#include "ely/export.h"

#ifdef __cplusplus
extern "C" {
#endif

ELY_EXPORT void _ely_assert_fail(const char*  assertion,
                                 const char*  msg,
                                 const char*  file,
                                 unsigned int line,
                                 const char*  fn) ELY_NORETURN;

ELY_EXPORT void _ely_unimplemented(const char*  msg,
                                   const char*  file,
                                   unsigned int line,
                                   const char*  fn) ELY_NORETURN;

#define ELY_UNIMPLEMENTED(msg)                                                 \
    _ely_unimplemented(msg, __FILE__, __LINE__, __PRETTY_FUNCTION__)

#ifdef __cplusplus
#define ELY_ASSERT_ALWAYS(cond, msg)                                           \
    (static_cast<bool>(cond) ?                                                 \
         static_cast<void>(0) :                                                \
         _ely_assert_fail(                                                     \
             #cond, msg, __FILE__, __LINE__, __PRETTY_FUNCTION__))
#else
#define ELY_ASSERT_ALWAYS(cond, msg)                                           \
    ((cond) ? void(0) :                                                        \
              _ely_assert_fail(                                                \
                  #cond, msg, __FILE__, __LINE__, __PRETTY_FUNCTION__))

#endif

#ifdef NDEBUG
#ifdef __cplusplus
#define ELY_ASSERT(cond, msg) static_cast<void>(0)
#else
#define ELY_ASSERT(cond, msg) ((void) (0))
#endif
#else
#define ELY_ASSERT(cond, msg) ELY_ASSERT_ALWAYS(cond, msg)
#endif

#ifdef __cplusplus
}
#endif
