#include "cli.hpp"

int main(int argc, char* argv[]) {
    return static_cast<int>(pax::run_cli(argc, argv));
}