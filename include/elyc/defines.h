#pragma once

#if defined(__has_attribute)
#if __has_attribute(musttail)
#define ELYC_MUSTTAIL __attribute__((musttail))
#endif
#endif

#if !defined(ELYC_MUSTTAIL)
#define ELYC_MUSTTAIL
#endif
