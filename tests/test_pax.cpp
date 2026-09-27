#include "cli.hpp"

#include <iostream>
#include <string>

namespace
{

bool expect_error(int argc, char *argv[])
{

    const auto result = pax::parse_cli(argc, argv);

    return result.status == pax::ParseStatus::error;
}

} // namespace

int main()
{
    {
        char arg0[] = "pax";
        char arg1[] = "create";
        char arg2[] = "backup.pax";
        char arg3[] = "file.txt";
        char arg4[] = "Documents";

        char *argv[] = {arg0, arg1, arg2, arg3, arg4};

        const auto result = pax::parse_cli(5, argv);

        if (result.status != pax::ParseStatus::ok)
        {
            std::cerr << "FAIL: basic create command\n";
            return 1;
        }

        if (result.options.command != pax::Command::create)
        {
            std::cerr << "FAIL: create command not parsed\n";
            return 1;
        }

        if (result.options.archive != "backup.pax")
        {
            std::cerr << "FAIL: archive path not parsed\n";
            return 1;
        }

        if (result.options.sources.size() != 2)
        {
            std::cerr << "FAIL: create sources count\n";
            return 1;
        }
    }

    {
        char arg0[] = "pax";
        char arg1[] = "create";
        char arg2[] = "--threads";
        char arg3[] = "8";
        char arg4[] = "--level";
        char arg5[] = "9";
        char arg6[] = "--verbose";
        char arg7[] = "backup.pax";
        char arg8[] = "Documents";

        char *argv[] = {arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8};

        const auto result = pax::parse_cli(9, argv);

        if (result.status != pax::ParseStatus::ok)
        {
            std::cerr << "FAIL: create options\n";
            return 1;
        }

        if (result.options.threads != 8)
        {
            std::cerr << "FAIL: threads\n";
            return 1;
        }

        if (result.options.compression_level != 9)
        {
            std::cerr << "FAIL: compression level\n";
            return 1;
        }

        if (!result.options.verbose)
        {
            std::cerr << "FAIL: verbose\n";
            return 1;
        }
    }

    {
        char arg0[] = "pax";
        char arg1[] = "create";
        char arg2[] = "--threads";
        char arg3[] = "0";
        char arg4[] = "backup.pax";
        char arg5[] = "file.txt";

        char *argv[] = {arg0, arg1, arg2, arg3, arg4, arg5};

        if (!expect_error(6, argv))
        {
            std::cerr << "FAIL: zero threads accepted\n";
            return 1;
        }
    }

    {
        char arg0[] = "pax";
        char arg1[] = "create";
        char arg2[] = "--level";
        char arg3[] = "10";
        char arg4[] = "backup.pax";
        char arg5[] = "file.txt";

        char *argv[] = {arg0, arg1, arg2, arg3, arg4, arg5};

        if (!expect_error(6, argv))
        {
            std::cerr << "FAIL: invalid compression level accepted\n";
            return 1;
        }
    }

    {
        char arg0[] = "pax";
        char arg1[] = "create";
        char arg2[] = "backup.pax";

        char *argv[] = {arg0, arg1, arg2};

        if (!expect_error(3, argv))
        {
            std::cerr << "FAIL: create without source accepted\n";
            return 1;
        }
    }

    {
        char arg0[] = "pax";
        char arg1[] = "something";

        char *argv[] = {arg0, arg1};

        if (!expect_error(2, argv))
        {
            std::cerr << "FAIL: unknown command accepted\n";
            return 1;
        }
    }

    {
        char arg0[] = "pax";
        char arg1[] = "--help";

        char *argv[] = {arg0, arg1};

        const auto result = pax::parse_cli(2, argv);

        if (result.status != pax::ParseStatus::help)
        {
            std::cerr << "FAIL: global help\n";
            return 1;
        }
    }

    {
        char arg0[] = "pax";
        char arg1[] = "create";
        char arg2[] = "--help";
        char arg3[] = "backup.pax";

        char *argv[] = {arg0, arg1, arg2, arg3};

        const auto result = pax::parse_cli(4, argv);

        if (result.status != pax::ParseStatus::help)
        {
            std::cerr << "FAIL: command help\n";
            return 1;
        }

        if (result.options.command != pax::Command::create)
        {
            std::cerr << "FAIL: command help lost command\n";
            return 1;
        }
    }

    {
        char arg0[] = "pax";
        char arg1[] = "list";
        char arg2[] = "--password";
        char arg3[] = "--password-file";
        char arg4[] = "password.txt";
        char arg5[] = "backup.pax";

        char *argv[] = {arg0, arg1, arg2, arg3, arg4, arg5};

        if (!expect_error(6, argv))
        {
            std::cerr << "FAIL: conflicting password options accepted\n";
            return 1;
        }
    }

    {
        char arg0[] = "pax";
        char arg1[] = "benchmark";
        char arg2[] = "Documents";
        char arg3[] = "Projects";
        char arg4[] = "photo.jpg";

        char *argv[] = {arg0, arg1, arg2, arg3, arg4};

        const auto result = pax::parse_cli(5, argv);

        if (result.status != pax::ParseStatus::ok)
        {
            std::cerr << "FAIL: benchmark sources\n";
            return 1;
        }

        if (result.options.sources.size() != 3)
        {
            std::cerr << "FAIL: benchmark source count\n";
            return 1;
        }
    }

    std::cout << "All tests passed\n";
    return 0;
}
