#include <cstdio>

#include <cinttypes>
#include <cstdlib>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include "lexer.hpp"
#include "stream.hpp"

int main(int argc, char** argv) {
  if (argc < 2) {
    std::fputs("ely: missing input\n", stderr);
    return EXIT_FAILURE;
  }

  // open file
  auto fdin = open(argv[1], O_RDONLY);
  if (fdin < 0) {
    std::fprintf(stderr, "can't open %s\n", argv[1]);
    return EXIT_FAILURE;
  }

  // get file length
  struct stat statbuf;
  if (fstat(fdin, &statbuf) < 0) {
    perror("fstat");
    return EXIT_FAILURE;
  }

  constexpr auto buffer_size = 64 * 1024;
  char* buffer = new char[buffer_size];

  ely::file_stream char_stream{argv[1], buffer, buffer_size};

  if (!char_stream) {
    std::fprintf(stderr, "failed to open \"%s\"\n", argv[1]);
    return EXIT_FAILURE;
  }

  // print lexical tokens
  auto lex = ely::lexer<ely::file_stream>{std::move(char_stream)};
  std::string tok_buffer;
  std::fputs("[\n", stdout);
  for (auto tok = lex.next(tok_buffer); tok.kind != ely::token_kind::eof;
       tok_buffer.clear(), tok = lex.next(tok_buffer)) {
    std::fprintf(stdout, "  (token :kind %" PRIu16 " :length %" PRIu16 ")\n",
                 static_cast<std::uint16_t>(tok.kind), tok.len);
  }
  std::fputs("]\n", stdout);
}
