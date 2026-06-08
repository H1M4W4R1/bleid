#include "help_command.h"
#include "reserve_command.h"
#include "uuid_command.h"

#include <exception>
#include <iostream>
#include <string>

namespace
{
void print_unknown_command(const char *program, const std::string &command)
{
    std::cerr << "Error: unknown command: " << command << "\n\n";
    print_help(program);
}
}

int main(int argc, char *argv[])
{
    try
    {
        if (argc < 2)
        {
            print_help(argv[0]);
            return 1;
        }

        const std::string command = argv[1];
        if (command == "help" || command == "--help" || command == "-h")
        {
            print_help(argv[0]);
            return 0;
        }

        if (command == "uuid")
        {
            return run_uuid_command(argv[0], argc - 1, argv + 1);
        }

        if (command == "reserve")
        {
            return run_reserve_command(argv[0], argc - 1, argv + 1);
        }

        if (command == "free")
        {
            return run_free_command(argv[0], argc - 1, argv + 1);
        }

        print_unknown_command(argv[0], command);
        return 1;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
