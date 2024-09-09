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
#include "parser.hpp"
#include "stream.hpp"

std::FILE* open_output_file(const char* outfile) {
  if (std::strcmp("-", outfile) == 0) {
    return stdout;
  }

  return std::fopen(outfile, "wb");
}

struct in_out_files {
  std::FILE* out_file;
  std::vector<std::FILE*> input_files;
};

in_out_files parse_in_out(std::span<char*> args) {
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
    return {};
  }

  std::FILE* out = open_output_file(outfile);

  if (!out) {
    std::fprintf(stderr, "ely: error: failed to open \"%s\" for writing\n",
                 outfile);
    return {};
  }

  std::vector<std::FILE*> input_streams;

  for (const char* file : inputfiles) {
    std::FILE* f = std::fopen(file, "rb");
    if (!f) {
      std::fprintf(stderr, "ely: error: failed to open \"%s\" for reading\n",
                   file);
      continue;
    }

    input_streams.push_back(f);
  }

  if (input_streams.empty()) {
    std::fputs("ely: error: no input files\n", stderr);
    return {};
  }

  return {out, std::move(input_streams)};
}

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
  } else {
    std::fprintf(stderr, "ely: error: unknown command \"%s\"\n", cmd);
    return EXIT_FAILURE;
  }
}

int execute_lex(std::span<char*> args) {
  auto in_out = parse_in_out(args);
  if (!in_out.out_file) {
    return EXIT_FAILURE;
  }

  std::FILE* out = in_out.out_file;

  constexpr auto buffer_size = 64 * 1024;
  char buffer[buffer_size];
  for (std::FILE* input_file : in_out.input_files) {
    ely::file_stream stream{input_file, buffer, buffer_size};

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

int execute_parse(std::span<char*> args) {
  auto in_out = parse_in_out(args);
  if (!in_out.out_file)
    return EXIT_FAILURE;

  std::FILE* out = in_out.out_file;

  constexpr auto buffer_size = 64 * 1024;
  char buffer[buffer_size];
  for (std::FILE* in : in_out.input_files) {
    ely::file_stream stream(in, buffer, buffer_size);

    std::string tok_buffer;
    auto lex = ely::lexer<ely::file_stream>{std::move(stream)};
    auto parse = ely::parser<ely::lexer<ely::file_stream>>{std::move(lex)};

    for (auto node = parse.next(tok_buffer); node != nullptr;
         node = parse.next(tok_buffer)) {
    }
  }
  return EXIT_SUCCESS;
}
