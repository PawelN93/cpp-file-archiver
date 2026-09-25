#include "cli.hpp"

#include <iostream>

int main() {
    char program_name[] = "pax";
    char help[] = "--help";
    char version[] = "--version";
    char unknown[] = "does-not-exist";
    char create[] = "create";

    {
        char* argv[] = {program_name, help};

        const auto result = pax::run_cli(2, argv);

        if (result != pax::ExitCode::success) {
            std::cerr << "FAIL: --help\n";
            return 1;
        }
    }

    {
        char* argv[] = {program_name, version};

        const auto result = pax::run_cli(2, argv);

        if (result != pax::ExitCode::success) {
            std::cerr << "FAIL: --version\n";
            return 1;
        }
    }

    {
        char* argv[] = {program_name, create};

        const auto result = pax::run_cli(2, argv);

        if (result != pax::ExitCode::runtime_error) {
            std::cerr << "FAIL: create command status\n";
            return 1;
        }
    }

    {
        char* argv[] = {program_name, unknown};

        const auto result = pax::run_cli(2, argv);

        if (result != pax::ExitCode::invalid_arguments) {
            std::cerr << "FAIL: unknown command status\n";
            return 1;
        }
    }

    {
        char* argv[] = {program_name};

        const auto result = pax::run_cli(1, argv);

        if (result != pax::ExitCode::success) {
            std::cerr << "FAIL: empty arguments\n";
            return 1;
        }
    }

    std::cout << "All tests passed\n";
    return 0;
}