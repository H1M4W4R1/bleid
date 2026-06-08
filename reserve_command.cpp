#include "reserve_command.h"
#include "help_command.h"
#include "reservation_store.h"
#include "uuid_generator.h"

#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace
{
struct UuidOptions
{
    UuidOptions()
        : base(DEFAULT_BASE_UUID),
          has_service(false),
          has_characteristic(false),
          has_descriptor(false),
          service(0),
          characteristic(0),
          descriptor(0)
    {
    }

    std::string base;
    bool has_service;
    bool has_characteristic;
    bool has_descriptor;
    unsigned int service;
    unsigned int characteristic;
    unsigned int descriptor;
};

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

std::string uuid_for_name(const std::string &name, const UuidOptions &options)
{
    return generate_uuid(options.base,
                         name,
                         options.has_service,
                         options.service,
                         options.has_characteristic,
                         options.characteristic,
                         options.has_descriptor,
                         options.descriptor);
}

bool find_uuid_collision(const ReservationStore &store,
                         const std::string &name,
                         const UuidOptions &options,
                         std::string &reserved_name,
                         std::string &uuid)
{
    uuid = uuid_for_name(name, options);

    const std::vector<std::string> names = store.names();
    for (std::vector<std::string>::const_iterator it = names.begin(); it != names.end(); ++it)
    {
        if (*it == name)
        {
            continue;
        }

        if (uuid_for_name(*it, options) == uuid)
        {
            reserved_name = *it;
            return true;
        }
    }

    return false;
}
}

int run_reserve_command(const char *program, int argc, char *argv[])
{
    bool check_only = false;
    bool list_only = false;
    bool has_uuid_options = false;
    UuidOptions uuid_options;

    try
    {
        std::string name;
        for (int i = 1; i < argc; ++i)
        {
            const std::string arg = argv[i];
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
            if (arg == "--base" || arg == "-b")
            {
                if (++i >= argc)
                {
                    throw std::runtime_error("--base requires a UUID value");
                }
                uuid_options.base = argv[i];
                has_uuid_options = true;
                continue;
            }
            if (arg == "--service" || arg == "--svc")
            {
                if (++i >= argc)
                {
                    throw std::runtime_error(arg + " requires a value");
                }
                uuid_options.service = static_cast<unsigned int>(parse_unsigned(argv[i], 0xFFFFFFFFULL, "service"));
                uuid_options.has_service = true;
                has_uuid_options = true;
                continue;
            }
            if (arg == "--characteristic" || arg == "--char")
            {
                if (++i >= argc)
                {
                    throw std::runtime_error(arg + " requires a value");
                }
                uuid_options.characteristic = static_cast<unsigned int>(parse_unsigned(argv[i], 0xFFFFFFFFULL, "characteristic"));
                uuid_options.has_characteristic = true;
                has_uuid_options = true;
                continue;
            }
            if (arg == "--descriptor" || arg == "--desc")
            {
                if (++i >= argc)
                {
                    throw std::runtime_error(arg + " requires a value");
                }
                uuid_options.descriptor = static_cast<unsigned int>(parse_unsigned(argv[i], 0xFFFFFFFFULL, "descriptor"));
                uuid_options.has_descriptor = true;
                has_uuid_options = true;
                continue;
            }

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

        if (list_only)
        {
            if (check_only)
            {
                throw std::runtime_error("--list cannot be combined with --check");
            }
            if (has_uuid_options)
            {
                throw std::runtime_error("--list cannot be combined with UUID options");
            }
            if (!name.empty())
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

        if (name.empty())
        {
            throw std::runtime_error("name is required");
        }

        ReservationStore store;
        const bool reserved = store.is_reserved(name);

        if (has_uuid_options && !check_only)
        {
            throw std::runtime_error("UUID options can only be used with --check");
        }

        if (check_only)
        {
            if (reserved)
            {
                std::cout << name << " is reserved" << std::endl;
                return 0;
            }

            std::string colliding_name;
            std::string uuid;
            if (find_uuid_collision(store, name, uuid_options, colliding_name, uuid))
            {
                std::cout << name << " is free by name, but collides with reserved name "
                          << colliding_name << " at UUID " << uuid << std::endl;
                return 0;
            }

            std::cout << name << " is free" << std::endl;
            return 1;
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
