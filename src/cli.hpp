#pragma once

#include <cstdint>

namespace pax {

enum class ExitCode : std::int32_t {
    success = 0,
    runtime_error = 1,
    invalid_arguments = 2
};

ExitCode run_cli(int argc, char* argv[]);

} // namespace pax