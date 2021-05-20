#pragma once

#if !defined(__has_feature)
#define __has_feature(x) 0
#endif

#if !defined(__has_extension)
#define __has_extension(x) 0
#endif

#if !defined(__has_attribute)
#define __has_attribute(x) 0
#endif

#if !defined(__has_builtin)
#define __has_builtin(x) 0
#endif

#if __has_attribute(musttail)
#define ELY_MUSTTAIL __attribute__((musttail))
#else
#define ELY_MUSTTAIL
#endif

#if !defined(ELY_MUSTTAIL)
#warning unimplemented attribute: ELY_MUSTTAIL
#define ELY_MUSTTAIL
#endif

#if __has_attribute(always_inline)
#define ELY_ALWAYS_INLINE inline __attribute__((always_inline))
#elif defined(_MSC_VER)
#define ELY_ALWAYS_INLINE inline __forceinline
#endif

#if !defined(ELY_ALWAYS_INLINE)
#warning unimplemented attribute: ELY_ALWAYS_INLINE
#define ELY_ALWAYS_INLINE inline
#endif

#if __has_attribute(fallthrough)
#define ELY_FALLTHROUGH __attribute__((fallthrough))
#endif

#if !defined(ELY_FALLTHROUGH)
#warning unimplemented attribute: ELY_FALLTHROUGH
#define ELY_FALLTHROUGH
#endif

#if __has_attribute(warn_unused_result)
#define ELY_NODISCARD __attribute__((warn_unused_result))
#elif defined(_MSC_VER)
#if _MSC_VER >= 1700
#define ELY_NODISCARD _Check_return_
#endif

#if !defined(ELY_NODISCARD)
#warning unimplemented attribute: ELY_NODISCARD
#define ELY_NODISCARD
#endif

#if __has_attribute(pure)
#define ELY_READONLY __attribute__((pure))
#endif

#if !defined(ELY_READONLY)
#warning unimplemented attribute: ELY_READONLY
#define ELY_READONLY
#endif

#if __has_attribute(const)
#define ELY_READNONE __attribute__((const))
#endif

#if !defined(ELY_READNONE)
#warning unimplemented attribute: ELY_READNONE
#define ELY_READNONE
#endif

#if __has_attribute(noinline)
#define ELY_NOINLINE __attribute__((noinline))
#elif defined(_MSC_VER)
#define ELY_NOINLINE __declspec(noinline)
#endif

#if !defined(ELY_NOINLINE)
#warning unimplemented attribute: ELY_NOINLINE
#define ELY_NOINLINE
#endif
