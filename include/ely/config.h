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

#if __has_attribute(preserve_none)
#define ELY_PRESERVE_NONE __attribute__((preserve_none))
#else
#define ELY_PELY_PRESERVE_NONE
#endif

#if __has_attribute(cold)
#define ELY_COLD __attribute__((cold))
#else
#define ELY_COLD
#endif

#if __has_attribute(hot)
#define ELY_HOT __attribute__((hot))
#else
#define ELY_HOT
#endif

#if __has_attribute(always_inline)
#define ELY_ALWAYS_INLINE inline __attribute__((always_inline))
#else
#define ELY_ALWAYS_INLINE inline
#endif

#if __has_attribute(noinline)
#define ELY_NOINLINE __attribute__((noinline))
#else
#define ELY_NOINLINE
#endif
