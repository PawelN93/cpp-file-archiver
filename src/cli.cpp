#include "cli.hpp"

#include <algorithm>
#include <charconv>
#include <iostream>
#include <limits>
#include <sstream>
#include <string_view>
#include <thread>

namespace pax
{
namespace
{

std::size_t default_thread_count()
{
    const unsigned int count = std::thread::hardware_concurrency();

    return std::max<std::size_t>(1, count);
}

bool parse_size(std::string_view text, std::size_t &value)
{

    std::uint64_t parsed = 0;

    const auto [ptr, error] = std::from_chars(text.data(), text.data() + text.size(), parsed);

    if (error != std::errc{} || ptr != text.data() + text.size() || parsed == 0 ||
        parsed > std::numeric_limits<std::size_t>::max())
    {
        return false;
    }

    value = static_cast<std::size_t>(parsed);
    return true;
}

bool parse_compression_level(std::string_view text, int &value)
{

    int parsed = 0;

    const auto [ptr, error] = std::from_chars(text.data(), text.data() + text.size(), parsed);

    if (error != std::errc{} || ptr != text.data() + text.size())
    {
        return false;
    }

    if (parsed < 0 || parsed > 9)
    {
        return false;
    }

    value = parsed;
    return true;
}

std::optional<Command> parse_command(std::string_view text)
{
    if (text == "create")
    {
        return Command::create;
    }

    if (text == "list")
    {
        return Command::list;
    }

    if (text == "extract")
    {
        return Command::extract;
    }

    if (text == "verify")
    {
        return Command::verify;
    }

    if (text == "benchmark")
    {
        return Command::benchmark;
    }

    return std::nullopt;
}

bool supports_password(Command command)
{
    return command == Command::create || command == Command::list || command == Command::extract ||
           command == Command::verify;
}

bool supports_threads(Command command)
{
    return command == Command::create || command == Command::benchmark;
}

bool supports_compression_level(Command command)
{
    return command == Command::create || command == Command::benchmark;
}

bool supports_force(Command command)
{
    return command == Command::create || command == Command::extract;
}

bool supports_verbose(Command command)
{
    return command == Command::create || command == Command::list || command == Command::extract ||
           command == Command::verify || command == Command::benchmark;
}

void print_global_help(std::ostream &output)
{
    output << "PAX - Secure File Archiver\n"
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

void print_command_help(std::ostream &output, Command command)
{

    switch (command)
    {
    case Command::create:
        output << "Usage:\n"
               << "  pax create [options] <archive.pax> <source> [source...]\n"
               << "\n"
               << "Options:\n"
               << "  --threads N          Number of compression workers\n"
               << "  --level N            Compression level (0-9)\n"
               << "  --password            Enable password protection\n"
               << "  --password-file FILE  Read password from file\n"
               << "  --force               Overwrite existing archive\n"
               << "  --verbose             Enable verbose output\n"
               << "  --help                Show this help message\n";
        break;

    case Command::list:
        output << "Usage:\n"
               << "  pax list [options] <archive.pax>\n"
               << "\n"
               << "Options:\n"
               << "  --password            Request archive password\n"
               << "  --password-file FILE  Read password from file\n"
               << "  --verbose             Enable verbose output\n"
               << "  --help                Show this help message\n";
        break;

    case Command::extract:
        output << "Usage:\n"
               << "  pax extract [options] <archive.pax> [output-directory]\n"
               << "\n"
               << "Options:\n"
               << "  --password            Request archive password\n"
               << "  --password-file FILE  Read password from file\n"
               << "  --force               Overwrite existing files\n"
               << "  --verbose             Enable verbose output\n"
               << "  --help                Show this help message\n";
        break;

    case Command::verify:
        output << "Usage:\n"
               << "  pax verify [options] <archive.pax>\n"
               << "\n"
               << "Options:\n"
               << "  --password            Request archive password\n"
               << "  --password-file FILE  Read password from file\n"
               << "  --verbose             Enable verbose output\n"
               << "  --help                Show this help message\n";
        break;

    case Command::benchmark:
        output << "Usage:\n"
               << "  pax benchmark [options] <source> [source...]\n"
               << "\n"
               << "Options:\n"
               << "  --threads N            Number of compression workers\n"
               << "  --level N              Compression level (0-9)\n"
               << "  --verbose              Enable verbose output\n"
               << "  --help                 Show this help message\n";
        break;

    case Command::none:
        print_global_help(output);
        break;
    }
}

void print_version(std::ostream &output)
{
    output << "pax 0.1.0\n";
}

} // namespace

ParseResult parse_cli(int argc, char *argv[])
{
    ParseResult result;

    if (argc <= 1)
    {
        result.status = ParseStatus::help;
        return result;
    }

    const std::string_view command_name{argv[1]};

    if (command_name == "--help" || command_name == "-h")
    {
        result.status = ParseStatus::help;
        return result;
    }

    if (command_name == "--version" || command_name == "-v")
    {
        result.status = ParseStatus::version;
        return result;
    }

    const auto command = parse_command(command_name);

    if (!command.has_value())
    {
        result.status = ParseStatus::error;
        result.error = "Unknown command: " + std::string{command_name};
        return result;
    }

    result.options.command = *command;
    result.options.threads = default_thread_count();

    std::vector<std::filesystem::path> positional_arguments;

    for (int i = 2; i < argc; ++i)
    {
        const std::string_view argument{argv[i]};

        if (argument == "--help" || argument == "-h")
        {
            result.status = ParseStatus::help;
            return result;
        }

        if (argument == "--version" || argument == "-v")
        {
            result.status = ParseStatus::version;
            return result;
        }

        if (argument == "--verbose")
        {
            if (!supports_verbose(*command))
            {
                result.status = ParseStatus::error;
                result.error = "--verbose is not supported for this command";
                return result;
            }

            result.options.verbose = true;
            continue;
        }

        if (argument == "--force")
        {
            if (!supports_force(*command))
            {
                result.status = ParseStatus::error;
                result.error = "--force is not supported for this command";
                return result;
            }

            result.options.force = true;
            continue;
        }

        if (argument == "--password")
        {
            if (!supports_password(*command))
            {
                result.status = ParseStatus::error;
                result.error = "--password is not supported for this command";
                return result;
            }

            if (result.options.password_file.has_value())
            {
                result.status = ParseStatus::error;
                result.error = "--password and --password-file cannot be used together";
                return result;
            }

            result.options.password = true;
            continue;
        }

        if (argument == "--password-file")
        {
            if (!supports_password(*command))
            {
                result.status = ParseStatus::error;
                result.error = "--password-file is not supported for this command";
                return result;
            }

            if (result.options.password)
            {
                result.status = ParseStatus::error;
                result.error = "--password and --password-file cannot be used together";
                return result;
            }

            if (i + 1 >= argc)
            {
                result.status = ParseStatus::error;
                result.error = "--password-file requires a file path";
                return result;
            }

            ++i;
            result.options.password_file = std::filesystem::path{argv[i]};

            continue;
        }

        if (argument == "--threads")
        {
            if (!supports_threads(*command))
            {
                result.status = ParseStatus::error;
                result.error = "--threads is not supported for this command";
                return result;
            }

            if (i + 1 >= argc)
            {
                result.status = ParseStatus::error;
                result.error = "--threads requires a positive integer";
                return result;
            }

            ++i;

            if (!parse_size(argv[i], result.options.threads))
            {
                result.status = ParseStatus::error;
                result.error = "--threads requires a positive integer";
                return result;
            }

            continue;
        }

        if (argument == "--level")
        {
            if (!supports_compression_level(*command))
            {
                result.status = ParseStatus::error;
                result.error = "--level is not supported for this command";
                return result;
            }

            if (i + 1 >= argc)
            {
                result.status = ParseStatus::error;
                result.error = "--level requires an integer from 0 to 9";
                return result;
            }

            ++i;

            if (!parse_compression_level(argv[i], result.options.compression_level))
            {
                result.status = ParseStatus::error;
                result.error = "--level requires an integer from 0 to 9";
                return result;
            }

            continue;
        }

        if (!argument.empty() && argument.front() == '-')
        {
            result.status = ParseStatus::error;
            result.error = "Unknown option: " + std::string{argument};
            return result;
        }

        positional_arguments.emplace_back(argument);
    }

    switch (*command)
    {
    case Command::create:
        if (positional_arguments.size() < 2)
        {
            result.status = ParseStatus::error;
            result.error = "create requires an archive path and at least one source";
            return result;
        }

        result.options.archive = positional_arguments.front();

        result.options.sources.assign(positional_arguments.begin() + 1, positional_arguments.end());

        break;

    case Command::list:
    case Command::verify:
        if (positional_arguments.size() != 1)
        {
            result.status = ParseStatus::error;
            result.error = "this command requires exactly one archive path";
            return result;
        }

        result.options.archive = positional_arguments.front();
        break;

    case Command::extract:
        if (positional_arguments.empty() || positional_arguments.size() > 2)
        {
            result.status = ParseStatus::error;
            result.error = "extract requires an archive path and optional output directory";
            return result;
        }

        result.options.archive = positional_arguments.front();

        if (positional_arguments.size() == 2)
        {
            result.options.output_directory = positional_arguments[1];
        }

        break;

    case Command::benchmark:
        if (positional_arguments.empty())
        {
            result.status = ParseStatus::error;
            result.error = "benchmark requires at least one source";
            return result;
        }

        result.options.sources = std::move(positional_arguments);
        break;

    case Command::none:
        result.status = ParseStatus::error;
        result.error = "No command specified";
        return result;
    }

    result.status = ParseStatus::ok;
    return result;
}

ExitCode run_cli(int argc, char *argv[])
{
    const ParseResult result = parse_cli(argc, argv);

    switch (result.status)
    {
    case ParseStatus::help:
        if (result.options.command == Command::none)
        {
            print_global_help(std::cout);
        }
        else
        {
            print_command_help(std::cout, result.options.command);
        }

        return ExitCode::success;

    case ParseStatus::version:
        print_version(std::cout);
        return ExitCode::success;

    case ParseStatus::error:
        std::cerr << "[pax] ERROR " << result.error << '\n';

        return ExitCode::invalid_arguments;

    case ParseStatus::ok:
        std::cerr << "[pax] ERROR Command not implemented yet: ";

        switch (result.options.command)
        {
        case Command::create:
            std::cerr << "create";
            break;

        case Command::list:
            std::cerr << "list";
            break;

        case Command::extract:
            std::cerr << "extract";
            break;

        case Command::verify:
            std::cerr << "verify";
            break;

        case Command::benchmark:
            std::cerr << "benchmark";
            break;

        case Command::none:
            break;
        }

        std::cerr << '\n';

        return ExitCode::runtime_error;
    }

    return ExitCode::runtime_error;
}

} // namespace pax
