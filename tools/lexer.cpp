#include <ely/lex/lexer.hpp>

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        return 1;
    }

    const char* input_file = argv[1];

    llvm::ErrorOr<std::unique_ptr<llvm::MemoryBuffer>> file_or_err =
        llvm::MemoryBuffer::getFile(input_file);

    if (std::error_code buffer_err = file_or_err.getError())
    {
        llvm::withColor::error(errs(), argv[0])
            << "Error reading " << input_file << ": " << buffer_err.message()
            << "\n";
    }

    
}