#pragma once

#include <stdint.h>

enum cont : uint8_t {
  CONT_START,
  CONT_WHITESPACE,
  CONT_TAB,
  CONT_NEWLINE_CR,
  CONT_IDENTIFIER,
};
