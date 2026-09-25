#include "cli.hpp"

#include <iostream>
#include <string_view>

namespace pax {
namespace {

void print_help() {
    std::cout
        << "PAX - Secure File Archiver\n"
        << "\n"
        << "Usage:\n"
        << "  pax <command> [options]\n"
        << "\n"
        << "Commands:\n"
        << "  create      Create a new archive\n"
        << "  list        List archive contents\n"
        << "  extract     Extract an archive\n"
        << "  verify      Verify archive integrity\n"
        << "  benchmark   Run compression benchmark\n"
        << "\n"
        << "Options:\n"
        << "  --help      Show this help message\n"
        << "  --version   Show version information\n";
}

void print_version() {
    std::cout << "pax 0.1.0\n";
}

} // namespace

ExitCode run_cli(int argc, char* argv[]) {
    if (argc <= 1) {
        print_help();
        return ExitCode::success;
    }

    const std::string_view command{argv[1]};

    if (command == "--help" || command == "-h") {
        print_help();
        return ExitCode::success;
    }

    if (command == "--version" || command == "-v") {
        print_version();
        return ExitCode::success;
    }

    if (
        command == "create" ||
        command == "list" ||
        command == "extract" ||
        command == "verify" ||
        command == "benchmark"
    ) {
        std::cerr
            << "[pax] ERROR Command not implemented yet: "
            << command << '\n';

        return ExitCode::runtime_error;
    }

    std::cerr
        << "[pax] ERROR Unknown command: "
        << command << '\n';

    return ExitCode::invalid_arguments;
}

} // namespace pax