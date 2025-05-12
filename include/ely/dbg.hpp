#pragma once

#ifndef ELY_DBG_VERBOSE
#define ELY_DBG_VERBOSE 0
#endif

#if ELY_DBG_VERBOSE
#define ELY_DBG(...) [&]() { __VA_ARGS__; }()
#else
// still get syntax validated, just not run
#define ELY_DBG(...) [&]() { __VA_ARGS__; }
#endif
