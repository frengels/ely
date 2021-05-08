#pragma once

#if defined(__has_attribute)
#if __has_attribute(musttail)
#define ELY_MUSTTAIL __attribute__((musttail))
#endif
#endif

#if !defined(ELY_MUSTTAIL)
#define ELY_MUSTTAIL
#endif
