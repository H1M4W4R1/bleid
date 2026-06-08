#include "help_command.h"
#include "reservation_store.h"
#include "uuid_generator.h"

#include <iostream>

void print_help(const char *program)
{
    std::cout
        << "Usage:\n"
        << "  " << program << " uuid [device-name] [options]\n"
        << "  " << program << " reserve [--check] [uuid-options] <name>\n"
        << "  " << program << " reserve --list\n"
        << "  " << program << " free <name>\n"
        << "  " << program << " help\n"
        << "\n"
        << "Commands:\n"
        << "  uuid      Generate a UUID from a device name or phrase.\n"
        << "  reserve   Reserve a name, or check if it is already reserved.\n"
        << "  free      Remove a reserved name.\n"
        << "  help      Show this help.\n"
        << "\n"
        << "Reservation file:\n"
        << "  " << default_reservation_path() << "\n"
        << "  Override with BLEID_RESERVATIONS.\n";
}

void print_uuid_help(const char *program)
{
    std::cout
        << "Usage: " << program << " uuid [base-template] [device-name] [--base UUID] [--service N] [--characteristic N] [--descriptor N] [--name TEXT]\n"
        << "\n"
        << "Options:\n"
        << "  --base, -b    Base UUID template. Default: " << DEFAULT_BASE_UUID << "\n"
        << "               A UUID-looking first positional argument can also be used as the base.\n"
        << "               Placeholders: S=service, T=characteristic, R=descriptor, X=hash.\n"
        << "               Placeholder matching is case-insensitive.\n"
        << "               Width is the total count of each placeholder in the base.\n"
        << "  --service     Service index as decimal or 0x-prefixed hexadecimal.\n"
        << "  --characteristic\n"
        << "               Characteristic index as decimal or 0x-prefixed hexadecimal.\n"
        << "  --descriptor  Descriptor index as decimal or 0x-prefixed hexadecimal.\n"
        << "  --name, -n    Device name/phrase. A positional value can be used instead.\n"
        << "  --help, -h    Show UUID command help.\n";
}

void print_reserve_help(const char *program)
{
    std::cout
        << "Usage:\n"
        << "  " << program << " reserve [--check] [uuid-options] <name>\n"
        << "  " << program << " reserve --list\n"
        << "\n"
        << "Options:\n"
        << "  --check, -c   Check whether the name or generated UUID is already reserved.\n"
        << "  --base, -b    Base UUID template used for collision detection.\n"
        << "  --service     Service index used for collision detection.\n"
        << "  --characteristic\n"
        << "               Characteristic index used for collision detection.\n"
        << "  --descriptor  Descriptor index used for collision detection.\n"
        << "  --list, -l    List all reserved names.\n"
        << "  --help, -h    Show reserve command help.\n";
}

void print_free_help(const char *program)
{
    std::cout
        << "Usage: " << program << " free <name>\n"
        << "\n"
        << "Options:\n"
        << "  --help, -h    Show free command help.\n";
}
