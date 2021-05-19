#pragma once

#if defined(__has_attribute)
#if __has_attribute(musttail)
#define ELY_MUSTTAIL __attribute__((musttail))
#else
#define ELY_MUSTTAIL
#endif
#endif

#if !defined(ELY_MUSTTAIL)
#warning unimplemented attribute: ELY_MUSTTAIL
#define ELY_MUSTTAIL
#endif

#if defined(__has_attribute)
#if __has_attribute(always_inline)
#define ELY_ALWAYS_INLINE inline __attribute__((always_inline))
#endif
#elif defined(_MSC_VER)
#define ELY_ALWAYS_INLINE inline __forceinline
#endif

#if !defined(ELY_ALWAYS_INLINE)
#warning unimplemented attribute: ELY_ALWAYS_INLINE
#define ELY_ALWAYS_INLINE inline
#endif

#if defined(__has_attribute)
#if __has_attribute(fallthrough)
#define ELY_FALLTHROUGH __attribute__((fallthrough))
#endif
#endif

#if !defined(ELY_FALLTHROUGH)
#warning unimplemented attribute: ELY_FALLTHROUGH
#define ELY_FALLTHROUGH
#endif

#if defined(__has_attribute)
#if __has_attribute(warn_unused_result)
#define ELY_NODISCARD __attribute__((warn_unused_result))
#endif
#elif defined(_MSC_VER)
#if _MSC_VER >= 1700
#define ELY_NODISCARD _Check_return_
#endif
#endif

#if !defined(ELY_NODISCARD)
#warning unimplemented attribute: ELY_NODISCARD
#define ELY_NODISCARD
#endif

#if defined(__has_attribute)
#if __has_attribute(pure)
#define ELY_PURE __attribute__((pure))
#endif
#endif

#if !defined(ELY_PURE)
#warning unimplemented attribute: ELY_PURE
#define ELY_PURE
#endif

#if defined(__has_attribute)
#if __has_attribute(const)
#define ELY_CONST __attribute__((const))
#endif
#endif

#if !defined(ELY_CONST)
#warning unimplemented attribute: ELY_CONST
#define ELY_CONST
#endif
