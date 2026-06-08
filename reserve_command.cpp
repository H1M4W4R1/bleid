#include "reserve_command.h"
#include "help_command.h"
#include "reservation_store.h"

#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace
{
std::string read_name_argument(int argc, char *argv[], int start)
{
    std::string name;
    for (int i = start; i < argc; ++i)
    {
        const std::string arg = argv[i];
        if (arg.size() > 0 && arg[0] == '-')
        {
            throw std::runtime_error("unknown argument: " + arg);
        }

        if (!name.empty())
        {
            name += " ";
        }
        name += arg;
    }

    if (name.empty())
    {
        throw std::runtime_error("name is required");
    }

    return name;
}
}

int run_reserve_command(const char *program, int argc, char *argv[])
{
    bool check_only = false;
    bool list_only = false;

    try
    {
        int name_start = 1;
        for (; name_start < argc; ++name_start)
        {
            const std::string arg = argv[name_start];
            if (arg == "--help" || arg == "-h")
            {
                print_reserve_help(program);
                return 0;
            }
            if (arg == "--check" || arg == "-c")
            {
                check_only = true;
                continue;
            }
            if (arg == "--list" || arg == "-l")
            {
                list_only = true;
                continue;
            }
            break;
        }

        if (list_only)
        {
            if (check_only)
            {
                throw std::runtime_error("--list cannot be combined with --check");
            }
            if (name_start < argc)
            {
                throw std::runtime_error("--list does not accept a name");
            }

            ReservationStore store;
            const std::vector<std::string> names = store.names();
            for (std::vector<std::string>::const_iterator it = names.begin(); it != names.end(); ++it)
            {
                std::cout << *it << std::endl;
            }
            return 0;
        }

        const std::string name = read_name_argument(argc, argv, name_start);
        ReservationStore store;
        const bool reserved = store.is_reserved(name);

        if (check_only)
        {
            std::cout << name << (reserved ? " is reserved" : " is free") << std::endl;
            return reserved ? 0 : 1;
        }

        if (!store.reserve(name))
        {
            std::cout << name << " is already reserved" << std::endl;
            return 1;
        }

        std::cout << name << " reserved" << std::endl;
        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << "\n\n";
        print_reserve_help(program);
        return 1;
    }
}

int run_free_command(const char *program, int argc, char *argv[])
{
    try
    {
        if (argc > 1)
        {
            const std::string first = argv[1];
            if (first == "--help" || first == "-h")
            {
                print_free_help(program);
                return 0;
            }
        }

        const std::string name = read_name_argument(argc, argv, 1);
        ReservationStore store;
        if (!store.free_name(name))
        {
            std::cout << name << " is not reserved" << std::endl;
            return 1;
        }

        std::cout << name << " freed" << std::endl;
        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << "\n\n";
        print_free_help(program);
        return 1;
    }
}
