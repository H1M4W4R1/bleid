#include "uuid_command.h"
#include "help_command.h"
#include "uuid_generator.h"

#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>

int run_uuid_command(const char *program, int argc, char *argv[])
{
    std::string base = DEFAULT_BASE_UUID;
    std::string phrase;
    bool has_service = false;
    bool has_characteristic = false;
    bool has_descriptor = false;
    unsigned int service = 0;
    unsigned int characteristic = 0;
    unsigned int descriptor = 0;

    try
    {
        for (int i = 1; i < argc; ++i)
        {
            const std::string arg = argv[i];

            if (arg == "--help" || arg == "-h")
            {
                print_uuid_help(program);
                return 0;
            }
            else if (arg == "--base" || arg == "-b")
            {
                if (++i >= argc)
                {
                    throw std::runtime_error("--base requires a UUID value");
                }
                base = argv[i];
            }
            else if (arg == "--service" || arg == "--svc")
            {
                if (++i >= argc)
                {
                    throw std::runtime_error(arg + " requires a value");
                }
                service = static_cast<unsigned int>(parse_unsigned(argv[i], 0xFFFULL, "service"));
                has_service = true;
            }
            else if (arg == "--characteristic" || arg == "--char")
            {
                if (++i >= argc)
                {
                    throw std::runtime_error(arg + " requires a value");
                }
                characteristic = static_cast<unsigned int>(parse_unsigned(argv[i], 0xFFFFULL, "characteristic"));
                has_characteristic = true;
            }
            else if (arg == "--descriptor" || arg == "--desc")
            {
                if (++i >= argc)
                {
                    throw std::runtime_error(arg + " requires a value");
                }
                descriptor = static_cast<unsigned int>(parse_unsigned(argv[i], 0xFFFULL, "descriptor"));
                has_descriptor = true;
            }
            else if (arg == "--name" || arg == "-n" || arg == "--phrase")
            {
                if (++i >= argc)
                {
                    throw std::runtime_error(arg + " requires a value");
                }
                phrase = argv[i];
            }
            else if (arg.size() > 0 && arg[0] == '-')
            {
                throw std::runtime_error("unknown argument: " + arg);
            }
            else
            {
                if (!phrase.empty())
                {
                    phrase += " ";
                }
                phrase += arg;
            }
        }

        std::cout << generate_uuid(base,
                                   phrase,
                                   has_service,
                                   service,
                                   has_characteristic,
                                   characteristic,
                                   has_descriptor,
                                   descriptor)
                  << std::endl;
        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << "\n\n";
        print_uuid_help(program);
        return 1;
    }
}
