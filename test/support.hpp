#pragma once

#ifndef __has_feature
#define __has_feature(x) 0
#endif

#define TEST_HAS_ADDRESS_SANITIZER __has_feature(address_sanitizer)
