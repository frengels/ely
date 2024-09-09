#include <cstdio>

#include <cinttypes>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include <span>
#include <vector>

#include "lexer.hpp"
#include "stream.hpp"

int execute_lex(std::span<char*> args);
int execute_parse(std::span<char*> args);

int main(int argc, char** argv) {
  if (argc < 2) {
    std::fputs("ely: missing command\n", stderr);
    return EXIT_FAILURE;
  }

  const char* cmd = argv[1];
  std::span<char*> args{argv + 2, static_cast<std::size_t>(argc - 2)};

  if (std::strcmp(cmd, "lex") == 0) {
    return execute_lex(args);
  } else if (std::strcmp(cmd, "parse") == 0) {
    return execute_parse(args);
  }

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
    std::fprintf(stdout, "  (token :kind %s :length %" PRIu16 ")\n",
                 token_kind_short_name(tok.kind), tok.len);
  }
  std::fputs("]\n", stdout);
}

std::FILE* open_output_file(const char* outfile) {
  if (std::strcmp("-", outfile) == 0) {
    return stdout;
  }

  return std::fopen(outfile, "wb");
}

int execute_lex(std::span<char*> args) {
  const char* outfile = nullptr;
  std::vector<const char*> inputfiles;

  for (unsigned i = 0; i != args.size(); ++i) {
    if (std::strcmp("-o", args[i]) == 0) {
      ++i;
      outfile = args[i];
    } else {
      inputfiles.push_back(args[i]);
    }
  }

  if (!outfile) {
    std::fputs("ely: error: no output file\n", stderr);
    return EXIT_FAILURE;
  }

  auto out = open_output_file(outfile);

  if (!out) {
    std::fprintf(stderr, "ely: error: failed to open \"%s\" for writing\n",
                 outfile);
    return EXIT_FAILURE;
  }

  if (inputfiles.empty()) {
    std::fputs("ely: error: no input files\n", stderr);
    return EXIT_FAILURE;
  }

  constexpr auto buffer_size = 64 * 1024;
  char buffer[buffer_size];
  for (const char* input_file : inputfiles) {
    ely::file_stream stream{input_file, buffer, buffer_size};

    if (!stream) {
      std::fprintf(stderr, "ely: error: failed to open \"%s\"\n", input_file);
      return EXIT_FAILURE;
    }

    auto lex = ely::lexer<ely::file_stream>{std::move(stream)};
    std::fputs("[\n", out);

    std::string tok_buffer;
    for (auto tok = lex.next(tok_buffer); tok.kind != ely::token_kind::eof;
         tok_buffer.clear(), tok = lex.next(tok_buffer)) {
      std::fprintf(out, "  (token :kind %s :length %" PRIu16 ")\n",
                   ely::token_kind_short_name(tok.kind), tok.len);
    }
    std::fputs("]\n", out);
  }

  return EXIT_SUCCESS;
}

int execute_parse(std::span<char*> args) { return EXIT_SUCCESS; }
