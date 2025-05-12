#pragma once

#ifndef __has_builtin
#define __has_builtin(x) 0
#endif

#ifndef __has_attribute
#define __has_attribute(x) 0
#endif

#if __has_attribute(musttail)
#define ELY_MUSTTAIL __attribute__((musttail))
#else
#define ELY_MUSTTAIL
#endif

#if __has_attribute(always_inline)
#define ELY_ALWAYS_INLINE __attribute__((always_inline))
#else
#define ELY_ALWAYS_INLINE
#endif
