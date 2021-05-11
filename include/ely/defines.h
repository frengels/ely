#pragma once

#if defined(__has_attribute)
#if __has_attribute(musttail)
#define ELY_MUSTTAIL __attribute__((musttail))
#endif
#endif

#if !defined(ELY_MUSTTAIL)
#define ELY_MUSTTAIL
#endif

#if defined(__has_attribute)
#if __has_attribute(always_inline)
#define ELY_ALWAYS_INLINE __attribute__((always_inline))
#endif
#endif

#if !defined(ELY_ALWAYS_INLINE)
#define ELY_ALWAYS_INLINE inline
#endif