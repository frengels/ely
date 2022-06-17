#include <llvm/Support/CommandLine.h>
#include <llvm/Support/Host.h>
#include <llvm/Support/InitLLVM.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/WithColor.h>

using namespace llvm;

static cl::opt<std::string>
    input_file(cl::Positional, cl::desc("<input file>"), cl::Required);

static cl::opt<std::string> output_filename("o",
                                            cl::desc("Specify output filename"),
                                            cl::value_desc("filename"));

static cl::opt<signed char> opt_level(
    cl::desc("Setting the optimization level."),
    cl::ZeroOrMore,
    cl::values(
        clEnumValN(3, "O", "Equivalent to -O3"),
        clEnumValN(0, "O0", "Optimization level 0"),
        clEnumValN(1, "O1", "Optimization level 1"),
        clEnumValN(2, "O2", "Optimization level 2"),
        clEnumValN(3, "O3", "Optimization level 3"),
        clEnumValN(-1, "Os", "Like -O2 with extra optimizations for size"),
        clEnumValN(-2, "Oz", "Like -Os but reduces code size further")),
    cl::init(0));

static cl::opt<std::string>
    mtriple("mtriple", cl::desc("Override target triple for module"));

static cl::opt<bool> emit_llvm("emit-llvm",
                               cl::desc("Emit IR instead of assembler"),
                               cl::init(false));

void print_version(llvm::raw_ostream& out)
{
    out << "elyc\n";
    out << "  Default target: " << llvm::sys::getDefaultTargetTriple() << "\n";
    auto cpu = llvm::sys::getHostCPUName().str();
    out << "  Host CPU: " << cpu << "\n";
    llvm::TargetRegistry::printRegisteredTargetsForVersion(out);
    out.flush();

    exit(EXIT_SUCCESS);
}

int main(int argc, char** argv)
{
    llvm::InitLLVM(argc, argv);

    llvm::InitializeAllTargets();
    // llvm::InitializeAllTargetMCs();
    // llvm::InitializeAllAsmPrinters();

    cl::SetVersionPrinter(&print_version);
    cl::ParseCommandLineOptions(argc, argv, "elyc - Ely Compiler");

    if (!input_file.empty())
    {
        llvm::ErrorOr<std::unique_ptr<llvm::MemoryBuffer>> file_or_err =
            llvm::MemoryBuffer::getFile(input_file);

        if (std::error_code buffer_err = file_or_err.getError())
        {
            llvm::WithColor::error(errs(), argv[0])
                << "Error reading " << input_file << ": "
                << buffer_err.message() << "\n";
        }

        llvm::SourceMgr src_mgr;
        src_mgr.AddNewSourceBuffer(std::move(*file_or_err), llvm::SMLoc());
    }
}