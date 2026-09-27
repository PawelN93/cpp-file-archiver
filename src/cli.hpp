#pragma once

#include <cstdint>
#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace pax
{

enum class ExitCode : std::int32_t
{
    success = 0,
    runtime_error = 1,
    invalid_arguments = 2
};

enum class Command
{
    none,
    create,
    list,
    extract,
    verify,
    benchmark
};

struct CliOptions
{
    Command command = Command::none;

    bool verbose = false;
    bool force = false;
    bool password = false;

    std::optional<std::filesystem::path> password_file;

    std::size_t threads = 1;
    int compression_level = 6;

    std::filesystem::path archive;
    std::filesystem::path output_directory = ".";

    std::vector<std::filesystem::path> sources;
};

enum class ParseStatus
{
    ok,
    help,
    version,
    error
};

struct ParseResult
{
    ParseStatus status = ParseStatus::error;
    CliOptions options;
    std::string error;
};

ParseResult parse_cli(int argc, char *argv[]);

ExitCode run_cli(int argc, char *argv[]);

} // namespace pax
